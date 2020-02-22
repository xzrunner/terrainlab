#include "wmv/SplatRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/VertexAttrib.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ShaderUniforms.h>
#include <painting3/Shader.h>
#include <wm/HeightField.h>
#include <wm/TextureBaker.h>
#include <model/TextureLoader.h>

namespace
{

const char* vs = R"(

#version 130

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform sampler2D u_heightmap;

varying float v_height;
varying vec2  v_texcoord;
varying vec3  v_fragpos;

void main()
{
    const float h_scale = 0.2;

	v_height = texture2D(u_heightmap, texcoord).a;

	vec4 pos = position;
	pos.y = v_height * h_scale;
	gl_Position = u_projection * u_view * u_model * pos;

	v_texcoord = texcoord;
    v_fragpos = vec3(u_model * pos);
}

)";

const char* fs = R"(

#version 130

uniform vec3 u_light_dir;

uniform sampler2D u_splatmap0;
uniform sampler2D u_splatmap1;
uniform sampler2D u_splatmap2;
uniform sampler2D u_splatmap3;

varying float v_height;
varying vec2  v_texcoord;
varying vec3  v_fragpos;

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)),
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }

vec4 textureNoTile1( sampler2D samp, in vec2 uv )
{
    ivec2 iuv = ivec2( floor( uv ) );
     vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );

    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );

    // uv's, and derivatives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;

    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );

    return mix( mix( textureGrad( samp, uva, ddxa, ddya ),
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ),
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

vec4 textureNoTile2( sampler2D samp, in vec2 uv )
{
    vec2 p = floor( uv );
    vec2 f = fract( uv );

    // derivatives (for correct mipmapping)
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // voronoi contribution
    vec4 va = vec4( 0.0 );
    float wt = 0.0;
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2 g = vec2( float(i), float(j) );
        vec4 o = hash4( p + g );
        vec2 r = g - f + o.xy;
        float d = dot(r,r);
        float w = exp(-5.0*d );
        vec4 c = textureGrad( samp, uv + o.zw, ddx, ddy );
        va += w*c;
        wt += w;
    }

    // normalization
    return va/wt;
}

//vec4 textureNoTile3( sampler2D samp, in vec2 uv )
//{
//    // sample variation pattern
//    float k = texture( iChannel1, 0.005*x ).x; // cheap (cache friendly) lookup
//
//    // compute index
//    float index = k*8.0;
//    float i = floor( index );
//    float f = fract( index );
//
//    // offsets for the different virtual patterns
//    vec2 offa = sin(vec2(3.0,7.0)*(i+0.0)); // can replace with any other hash
//    vec2 offb = sin(vec2(3.0,7.0)*(i+1.0)); // can replace with any other hash
//
//    // compute derivatives for mip-mapping
//    vec2 dx = dFdx(x), dy = dFdy(x);
//
//    // sample the two closest virtual patterns
//    vec3 cola = textureGrad( iChannel0, x + offa, dx, dy ).xxx;
//    vec3 colb = textureGrad( iChannel0, x + offb, dx, dy ).xxx;
//
//    float sum( vec3 v ) { return v.x+v.y+v.z; }
//
//    // interpolate between the two virtual patterns
//    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*sum(cola-colb)) );
//
//}

vec4 TexSample(sampler2D samp, in vec3 world_pos, in vec3 weights)
{
    const float uv_scale = 16;
    vec3 uv = uv_scale * world_pos;
	vec3 blended = weights.xxx * textureNoTile2(samp, uv.yz).rgb +
	               weights.yyy * textureNoTile2(samp, uv.zx).rgb +
	               weights.zzz * textureNoTile2(samp, uv.xy).rgb;
    return vec4(blended, 1.0);
}

void main()
{
    // normal
    vec3 fdx = dFdx(v_fragpos);
    vec3 fdy = dFdy(v_fragpos);
    vec3 N = normalize(cross(fdx, fdy));

    float h = v_fragpos.y;
    vec4 real_height = vec4(h, h, h, h);

    const float height[4] = { 0, 0.2, 0.12, 0.14 };
    const bool height_up[4] = { false, true, false, true };
    const float height_var[4] = { 0, 0.05, 0.2, 0.01 };
    const float layer2Lo = height[1] - (height_up[1] ? 0.5f : -0.5f) * max(0.001f, height_var[1]);
    const float layer2Hi = height[1] + (height_up[1] ? 0.5f : -0.5f) * max(0.001f, height_var[1]);
    const float layer3Lo = height[2] - (height_up[2] ? 0.5f : -0.5f) * max(0.001f, height_var[2]);
    const float layer3Hi = height[2] + (height_up[2] ? 0.5f : -0.5f) * max(0.001f, height_var[2]);
    const float layer4Lo = height[3] - (height_up[3] ? 0.5f : -0.5f) * max(0.001f, height_var[3]);
    const float layer4Hi = height[3] + (height_up[3] ? 0.5f : -0.5f) * max(0.001f, height_var[3]);

    const vec4 layer_height_scale = vec4(
        1.0f / (layer2Hi - layer2Lo), // weight of layer2 (sand)
        1.0f / (layer3Hi - layer3Lo),  // weight of layer3 (rock)
        1.0f / (layer4Hi - layer4Lo), // weight of layer4 (snow)
        0.0f
    );
    const vec4 layer_height_bias = vec4(
        -layer2Lo / (layer2Hi - layer2Lo), // weight of layer2 (sand)
        -layer3Lo / (layer3Hi - layer3Lo),  // weight of layer3 (rock)
        -layer4Lo / (layer4Hi - layer4Lo), // weight of layer4 (snow)
        0.0f
    );

    vec4 lerp_weights = clamp(real_height * layer_height_scale + layer_height_bias, 0.0, 1.0);

    vec3 lerp_weights_norm = N * N;

    vec3 splat_col = TexSample(u_splatmap0, v_fragpos, lerp_weights_norm).rgb;
    splat_col = mix(splat_col, TexSample(u_splatmap1, v_fragpos, lerp_weights_norm).rgb, lerp_weights.x);
    splat_col = mix(splat_col, TexSample(u_splatmap2, v_fragpos, lerp_weights_norm).rgb, lerp_weights.y);
    splat_col = mix(splat_col, TexSample(u_splatmap3, v_fragpos, lerp_weights_norm).rgb, lerp_weights.z);

    // N dot L
    vec3 light_dir = normalize(u_light_dir);
    float n_dot_l = clamp(dot(N, light_dir), 0, 1);

    vec4 sky_col = vec4(0.48, 0.52, 0.6, 0.0);
    float sky_overbright = 1.0;
    vec4 sun_col = vec4(1.0, 0.9, 0.7, 0.0);
    float sub_overbright = 1.6;

    vec4 diff_col = mix(sky_col * sky_overbright, sun_col * sub_overbright, n_dot_l);
    vec3 color = diff_col.rgb * splat_col;

    gl_FragColor = vec4(color, 1.0);
}

)";

}

namespace wmv
{

SplatRenderer::SplatRenderer()
{
    InitTextuers();
    InitShader();
}

void SplatRenderer::Setup(const std::shared_ptr<wm::HeightField>& hf)
{
    if (m_shaders.empty()) {
        return;
    }
    assert(m_shaders.size() == 1);
    auto shader = m_shaders.front();

    m_hf = hf;
    if (!m_hf) {
        return;
    }
    assert(hf);
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    auto old = m_height_map;
    m_height_map = hf->GetHeightmap();

    // textures
    if (m_height_map != old)
    {
        std::vector<uint32_t> texture_ids;
        texture_ids.reserve(6);
        texture_ids.push_back(m_height_map->TexID());
        //texture_ids.push_back(m_detail_map->TexID());
        texture_ids.push_back(m_splat_map[0]->TexID());
        texture_ids.push_back(m_splat_map[1]->TexID());
        texture_ids.push_back(m_splat_map[2]->TexID());
        texture_ids.push_back(m_splat_map[3]->TexID());

        shader->SetUsedTextures(texture_ids);
    }

    // vertex buffer
    if (!old ||
        old->Width() != m_height_map->Width() ||
        old->Height() != m_height_map->Height()) {
        BuildVertBuf();
    }

    // bind shader
    shader->Use();

    // update uniforms
    pt0::ShaderUniforms vals;
    vals.AddVar("u_light_dir", pt0::RenderVariant(sm::vec3(1, -1, 2)));
    vals.Bind(*shader);
}

void SplatRenderer::Draw() const
{
    if (m_shaders.empty() || !m_hf) {
        return;
    }

    DrawVertBuf();
}

void SplatRenderer::Clear()
{
    m_height_map.reset();
}

void SplatRenderer::InitTextuers()
{
    //m_detail_map = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\detailMap.tga");

    m_splat_map[0] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\dark_dirt.jpg");
    m_splat_map[1] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\rock.jpg");
    m_splat_map[2] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\grass.jpg");
    m_splat_map[3] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\snow.png");
}

void SplatRenderer::InitShader()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<ur::VertexAttrib> layout;
    layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    rc.CreateVertexLayout(layout);

    std::vector<std::string> texture_names;
    texture_names.push_back("u_heightmap");
    texture_names.push_back("u_splatmap0");
    texture_names.push_back("u_splatmap1");
    texture_names.push_back("u_splatmap2");
    texture_names.push_back("u_splatmap3");

    pt3::Shader::Params sp(texture_names, layout);
    sp.vs = vs;
    sp.fs = fs;

    sp.uniform_names.Add(pt0::UniformTypes::ModelMat, rp::MODEL_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  rp::VIEW_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  rp::PROJ_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::CamPos, sw::node::CameraPos::CamPosName());

    auto shader = std::make_shared<pt3::Shader>(&rc, sp);
    m_shaders.push_back(shader);
}

}