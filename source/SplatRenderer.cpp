#include "terrainlab/SplatRenderer.h"

#include <heightfield/HeightField.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Texture.h>
#include <unirender/ComponentDataType.h>
#include <unirender/VertexBufferAttribute.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ShaderUniforms.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/Shader.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <terraingraph/TextureBaker.h>
#include <model/TextureLoader.h>

#include <algorithm>

namespace
{

const char* vs = R"(

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform vec2 u_inv_res;

uniform sampler2D u_heightmap;

varying float v_height;
#ifdef BUILD_NORMAL_MAP
varying vec2  v_texcoord;
#endif // BUILD_NORMAL_MAP
varying vec3  v_fragpos;
varying vec3  v_normal;

vec3 ComputeNormalCentralDifference(vec2 position, float heightExaggeration)
{
    float leftHeight = texture(u_heightmap, position - vec2(1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float rightHeight = texture(u_heightmap, position + vec2(1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float bottomHeight = texture(u_heightmap, position - vec2(0.0, 1.0) * u_inv_res).r * heightExaggeration;
    float topHeight = texture(u_heightmap, position + vec2(0.0, 1.0) * u_inv_res).r * heightExaggeration;
    return normalize(vec3(leftHeight - rightHeight, 2.0, bottomHeight - topHeight));
}

vec3 ComputeNormalSobelFilter(vec2 position, float heightExaggeration)
{
    float upperLeft = texture(u_heightmap, position + vec2(-1.0, 1.0) * u_inv_res).r * heightExaggeration;
    float upperCenter = texture(u_heightmap, position + vec2(0.0, 1.0) * u_inv_res).r * heightExaggeration;
    float upperRight = texture(u_heightmap, position + vec2(1.0, 1.0) * u_inv_res).r * heightExaggeration;
    float left = texture(u_heightmap, position + vec2(-1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float right = texture(u_heightmap, position + vec2(1.0, 0.0) * u_inv_res).r * heightExaggeration;
    float lowerLeft = texture(u_heightmap, position + vec2(-1.0, -1.0) * u_inv_res).r * heightExaggeration;
    float lowerCenter = texture(u_heightmap, position + vec2(0.0, -1.0) * u_inv_res).r * heightExaggeration;
    float lowerRight = texture(u_heightmap, position + vec2(1.0, -1.0) * u_inv_res).r * heightExaggeration;

    float x = upperRight + (2.0 * right) + lowerRight - upperLeft - (2.0 * left) - lowerLeft;
    float y = lowerLeft + (2.0 * lowerCenter) + lowerRight - upperLeft - (2.0 * upperCenter) - upperRight;

    return normalize(vec3(-x, 1.0, y));
}

void main()
{
    const float h_scale = 0.2;
    //const float h_scale = 2;

	v_height = texture2D(u_heightmap, texcoord).r;

	vec4 pos = position;
	pos.y = v_height * h_scale;
	gl_Position = u_projection * u_view * u_model * pos;

#ifdef BUILD_NORMAL_MAP
    v_texcoord = texcoord;
#endif // BUILD_NORMAL_MAP
    v_fragpos = vec3(u_model * pos);

    v_normal = ComputeNormalSobelFilter(texcoord, 100.0);
}

)";

const char* fs = R"(

#define MULTIPROJECT
//#define TEX_NO_TILE

uniform vec3 u_light_dir;

uniform vec4 u_layer_height_scale;
uniform vec4 u_layer_height_bias;
uniform vec4 u_layer_slope_scale;
uniform vec4 u_layer_slope_bias;
uniform mat4 u_layer_distort;

uniform sampler2D u_noise_map;

uniform sampler2D u_splatmap0;
uniform sampler2D u_splatmap1;
uniform sampler2D u_splatmap2;
uniform sampler2D u_splatmap3;

#ifdef BUILD_NORMAL_MAP
uniform sampler2D u_normal_map;
varying vec2 v_texcoord;
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
uniform sampler2D u_shadow_map;
#endif // BUILD_SHADOW_MAP

varying float v_height;
varying vec3  v_fragpos;
varying vec3  v_normal;

const float UV_SCALE = 32.0;

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

#ifdef TEX_NO_TILE
vec4 TexSampleMultiProj(sampler2D samp, in vec3 world_pos, in vec3 weights)
{
    vec3 uv = UV_SCALE * world_pos;
	vec3 blended = weights.xxx * textureNoTile2(samp, uv.yz).rgb +
	               weights.yyy * textureNoTile2(samp, uv.zx).rgb +
	               weights.zzz * textureNoTile2(samp, uv.xy).rgb;
    return vec4(blended, 1.0);
}
#else
vec4 TexSampleMultiProj(sampler2D samp, in vec3 world_pos, in vec3 weights)
{
    vec3 uv = UV_SCALE * world_pos;
	vec3 blended = weights.xxx * texture(samp, uv.yz).rgb +
	               weights.yyy * texture(samp, uv.zx).rgb +
	               weights.zzz * texture(samp, uv.xy).rgb;
    return vec4(blended, 1.0);
}
#endif // TEX_NO_TILE

void main()
{
    if (v_height == 0) {
        gl_FragColor = vec4(0.05, 0.05, 0.95, 1.0);
        return;
    }

//#ifdef BUILD_NORMAL_MAP
//    // fixme
//    //vec3 N = texture2D(u_normal_map, v_texcoord).rgb;
//    vec3 N = normalize(texture2D(u_normal_map, v_texcoord).rgb);
//#else
//    vec3 fdx = dFdx(v_fragpos);
//    vec3 fdy = dFdy(v_fragpos);
//    vec3 N = normalize(cross(fdx, fdy));
//#endif // BUILD_NORMAL_MAP
    vec3 N = v_normal;

#ifdef TEX_NO_TILE
    vec4 global_col = textureNoTile2(u_noise_map, UV_SCALE * v_fragpos.xz);
#else
    vec4 global_col = texture(u_noise_map, UV_SCALE * v_fragpos.xz);
#endif // TEX_NO_TILE
    vec4 distortion = u_layer_distort * (global_col - 0.5) * 0;

    vec4 real_height = v_height * 0.5 + 0.5 + distortion * 0.03;
    vec4 lerp_weights = clamp(real_height * u_layer_height_scale + u_layer_height_bias, 0.0, 1.0);
    vec4 lerp_weights2 = clamp((distortion.w + length(vec2(N.x, N.z))) * u_layer_slope_scale + u_layer_slope_bias, 0.0, 1.0);
    lerp_weights = lerp_weights * lerp_weights2;

#ifdef MULTIPROJECT
    vec3 lerp_weights_norm = N * N;
    vec3 splat_col = TexSampleMultiProj(u_splatmap0, v_fragpos, lerp_weights_norm).rgb;
    splat_col = mix(splat_col, TexSampleMultiProj(u_splatmap1, v_fragpos, lerp_weights_norm).rgb, lerp_weights.x);
    splat_col = mix(splat_col, TexSampleMultiProj(u_splatmap2, v_fragpos, lerp_weights_norm).rgb, lerp_weights.y);
    splat_col = mix(splat_col, TexSampleMultiProj(u_splatmap3, v_fragpos, lerp_weights_norm).rgb, lerp_weights.z);
#else
    vec2 tex_coord = UV_SCALE * v_fragpos.xz;
    vec3 splat_col = texture(u_splatmap0, tex_coord).rgb;
    splat_col = lerp(color, texture(u_splatmap1, tex_coord).rgb, lerp_weights.x);
    splat_col = lerp(color, texture(u_splatmap2, tex_coord).rgb, lerp_weights.y);
    splat_col = lerp(color, texture(u_splatmap3, tex_coord).rgb, lerp_weights.z);
#endif // MULTIPROJECT

    // N dot L
    vec3 light_dir = normalize(u_light_dir);
    float n_dot_l = clamp(dot(N, light_dir), 0, 1);

    vec4 sky_col = vec4(0.48, 0.52, 0.6, 0.0);
    float sky_overbright = 1.0;
    vec4 sun_col = vec4(1.0, 0.9, 0.7, 0.0);
    float sub_overbright = 1.6;

#ifdef BUILD_SHADOW_MAP
    vec3 shadow = texture2D(u_shadow_map, v_texcoord).rgb;
#endif

    vec3 env_col = mix(sky_col * sky_overbright, sun_col * sub_overbright, n_dot_l).rgb;
#ifdef BUILD_SHADOW_MAP
    vec3 color = env_col * splat_col * shadow;
#else
    vec3 color = env_col * splat_col;
#endif // BUILD_SHADOW_MAP

    gl_FragColor = vec4(color, 1.0);
}

)";

}

namespace terrainlab
{

SplatRenderer::SplatRenderer(const ur::Device& dev)
    : rp::HeightfieldRenderer(dev)
{
    InitTextuers(dev);
    InitShader(dev);
    InitUniforms();
}

void SplatRenderer::Setup(const ur::Device& dev, ur::Context& ctx,
                          const std::shared_ptr<hf::HeightField>& hf)
{
    if (m_shaders.empty()) {
        return;
    }

    m_hf = hf;
    if (!m_hf) {
        return;
    }
    assert(hf);
    auto old = m_height_map;
    m_height_map = hf->GetHeightmap(dev);

#ifdef BUILD_NORMAL_MAP
    m_normal_map = terraingraph::TextureBaker::GenNormalMap(*hf, rc);
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
    m_shadow_map = terraingraph::TextureBaker::GenShadowMap(*hf, rc, m_light_dir);
#endif // BUILD_SHADOW_MAP

//    // textures
//    if (m_height_map != old)
//    {
//        ctx.SetTexture(shader->QueryTexSlot("u_heightmap"), m_height_map);
//#ifdef BUILD_NORMAL_MAP
//        ctx.SetTexture(shader->QueryTexSlot("u_normal_map"), m_normal_map);
//#endif // BUILD_NORMAL_MAP
//#ifdef BUILD_SHADOW_MAP
//        ctx.SetTexture(shader->QueryTexSlot("u_shadow_map"), m_shadow_map);
//#endif // BUILD_SHADOW_MAP
//        ctx.SetTexture(shader->QueryTexSlot("u_noise_map"), m_noise_map);
//        ctx.SetTexture(shader->QueryTexSlot("u_splatmap0"), m_splat_map[0]);
//        ctx.SetTexture(shader->QueryTexSlot("u_splatmap1"), m_splat_map[1]);
//        ctx.SetTexture(shader->QueryTexSlot("u_splatmap2"), m_splat_map[2]);
//        ctx.SetTexture(shader->QueryTexSlot("u_splatmap3"), m_splat_map[3]);
//    }

    // vertex buffer
    if (!old ||
        old->GetWidth() != m_height_map->GetWidth() ||
        old->GetHeight() != m_height_map->GetHeight()) {
        BuildVertBuf(ctx);
    }

    // bind shader
    auto shader = m_shaders.front();
//    shader->Bind();

    // update uniforms
    pt0::ShaderUniforms vals;
    sm::vec2 inv_res(1.0f / m_height_map->GetWidth(), 1.0f / m_height_map->GetHeight());
    vals.AddVar("u_inv_res",            pt0::RenderVariant(inv_res));
    vals.AddVar("u_light_dir",          pt0::RenderVariant(m_light_dir));
    vals.AddVar("u_layer_height_scale", pt0::RenderVariant(m_layer_height_scale));
    vals.AddVar("u_layer_height_bias",  pt0::RenderVariant(m_layer_height_bias));
    vals.AddVar("u_layer_slope_scale",  pt0::RenderVariant(m_layer_slope_scale));
    vals.AddVar("u_layer_slope_bias",   pt0::RenderVariant(m_layer_slope_bias));
    vals.AddVar("u_layer_distort",      pt0::RenderVariant(m_layer_distort));
    vals.Bind(*shader);
}

void SplatRenderer::Clear()
{
    HeightfieldRenderer::Clear();

    m_height_map.reset();
}

void SplatRenderer::BeforeDraw(ur::Context& ctx) const
{
    assert(m_shaders.size() == 1);
    auto shader = m_shaders.front();

    ctx.SetTexture(shader->QueryTexSlot("u_heightmap"), m_height_map);
#ifdef BUILD_NORMAL_MAP
    ctx.SetTexture(shader->QueryTexSlot("u_normal_map"), m_normal_map);
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
    ctx.SetTexture(shader->QueryTexSlot("u_shadow_map"), m_shadow_map);
#endif // BUILD_SHADOW_MAP
    ctx.SetTexture(shader->QueryTexSlot("u_noise_map"), m_noise_map);
    ctx.SetTexture(shader->QueryTexSlot("u_splatmap0"), m_splat_map[0]);
    ctx.SetTexture(shader->QueryTexSlot("u_splatmap1"), m_splat_map[1]);
    ctx.SetTexture(shader->QueryTexSlot("u_splatmap2"), m_splat_map[2]);
    ctx.SetTexture(shader->QueryTexSlot("u_splatmap3"), m_splat_map[3]);
}

void SplatRenderer::InitTextuers(const ur::Device& dev)
{
    const std::string filedir = "D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\";
    const int mipmap_levels = 32;

    auto noise_path = filedir + "fractalnoise.jpg";
    m_noise_map = model::TextureLoader::LoadFromFile(dev, noise_path.c_str());

    const std::string filenames[4] = {
        "dark_dirt.jpg",
        "rock.jpg",
        "grass.jpg",
        "snow.png",
    };
    for (size_t i = 0; i < 4; ++i) {
        std::string filepath = filedir + filenames[i];
        m_splat_map[i] = model::TextureLoader::LoadFromFile(dev, filepath.c_str(), mipmap_levels);
    }
}

void SplatRenderer::InitShader(const ur::Device& dev)
{
    std::vector<std::shared_ptr<ur::VertexBufferAttribute>> vbuf_attrs(2);
    // rp::VERT_POSITION_NAME
    vbuf_attrs[0] = std::make_shared<ur::VertexBufferAttribute>(
        ur::ComponentDataType::Float, 3, 0, 20
    );
    // rp::VERT_TEXCOORD_NAME
    vbuf_attrs[1] = std::make_shared<ur::VertexBufferAttribute>(
        ur::ComponentDataType::Float, 2, 12, 20
    );
    m_va->SetVertexBufferAttrs(vbuf_attrs);

    std::string _vs(vs);
    std::string _fs(fs);
#ifdef BUILD_NORMAL_MAP
    _vs = "#version 130\n#define BUILD_NORMAL_MAP\n" + _vs;
    _fs = "#version 130\n#define BUILD_NORMAL_MAP\n" + _fs;
#else
    _vs = "#version 130\n" + _vs;
    _fs = "#version 130\n" + _fs;
#endif // BUILD_NORMAL_MAP
    auto shader = dev.CreateShaderProgram(_vs, _fs);
    shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*shader, rp::MODEL_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*shader, rp::VIEW_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*shader, rp::PROJ_MAT_NAME));
    m_shaders.push_back(shader);
}

void SplatRenderer::InitUniforms()
{
    // height

    const float height[4]     = { 0.0f, 1.0f, 0.6f, 0.8f };
    const bool  height_up[4]  = { false, true, false, true };
    const float height_var[4] = { 0.0f, 0.1f, 0.32f, 0.02f };

    const float height2_lo = height[1] - (height_up[1] ? 0.5f : -0.5f) * std::max(0.001f, height_var[1]);
    const float height2_hi = height[1] + (height_up[1] ? 0.5f : -0.5f) * std::max(0.001f, height_var[1]);
    const float height3_lo = height[2] - (height_up[2] ? 0.5f : -0.5f) * std::max(0.001f, height_var[2]);
    const float height3_hi = height[2] + (height_up[2] ? 0.5f : -0.5f) * std::max(0.001f, height_var[2]);
    const float height4_lo = height[3] - (height_up[3] ? 0.5f : -0.5f) * std::max(0.001f, height_var[3]);
    const float height4_hi = height[3] + (height_up[3] ? 0.5f : -0.5f) * std::max(0.001f, height_var[3]);

    m_layer_height_scale = sm::vec4(
        1.0f / (height2_hi - height2_lo), // weight of layer2 (rock)
        1.0f / (height3_hi - height3_lo), // weight of layer3 (grass)
        1.0f / (height4_hi - height4_lo), // weight of layer4 (snow)
        0.0f
    );
    m_layer_height_bias = sm::vec4(
        -height2_lo / (height2_hi - height2_lo), // weight of layer2 (rock)
        -height3_lo / (height3_hi - height3_lo), // weight of layer3 (grass)
        -height4_lo / (height4_hi - height4_lo), // weight of layer4 (snow)
        0.0f
    );

    // slope

    const float slope[4]     = { 0, 23, 8, 34 };
    const bool  slope_up[4]  = { false, true, false, false };
    const float slope_var[4] = { 0, 68, 72, 140 };
	const float slope2_lo_tan = std::tan(0.01745f * (slope[1] - (slope_up[1] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[1])));
	const float slope2_hi_tan = std::tan(0.01745f * (slope[1] + (slope_up[1] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[1])));
	const float slope3_lo_tan = std::tan(0.01745f * (slope[2] - (slope_up[2] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[2])));
	const float slope3_hi_tan = std::tan(0.01745f * (slope[2] + (slope_up[2] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[2])));
	const float slope4_lo_tan = std::tan(0.01745f * (slope[3] - (slope_up[3] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[3])));
	const float slope4_hi_tan = std::tan(0.01745f * (slope[3] + (slope_up[3] ? 0.5f : -0.5f) * std::max(0.001f, slope_var[3])));

    m_layer_slope_scale = sm::vec4(
		1.0f / (slope2_hi_tan - slope2_lo_tan),  // weight of layer2 (rock)
		1.0f / (slope3_hi_tan - slope3_lo_tan),  // weight of layer3 (grass)
		1.0f / (slope4_hi_tan - slope4_lo_tan),  // weight of layer4 (snow)
		0.0f);

    m_layer_slope_bias = sm::vec4(
		-slope2_lo_tan / (slope2_hi_tan - slope2_lo_tan),  // weight of layer2 (rock)
		-slope3_lo_tan / (slope3_hi_tan - slope3_lo_tan),  // weight of layer3 (grass)
		-slope4_lo_tan / (slope4_hi_tan - slope4_lo_tan),  // weight of layer4 (snow)
		0.0);

    // distort

    const float distort_hi[4]  = { 0, 218.001f, 1.001f, 1.001f };
    const float distort_lo[4]  = { 0, 0.001f,   3.001f, 2.001f };
    const float distort_mid[4] = { 0, 13.001f,  2.001f, 0.001f };
    const float slope_distort_hi  = 5.001f;
    const float slope_distort_lo  = 28.001f;
    const float slope_distort_mid = 14.001f;
    auto m = m_layer_distort.x;
    m[0] = 10.0f * distort_hi[1];    m[1] = 10.0f * distort_mid[1];   m[2] = 10.0f * distort_lo[1];    m[3] = 0.0f;
    m[4] = 10.0f * distort_hi[2];    m[5] = 10.0f * distort_mid[2];   m[6] = 10.0f * distort_lo[2];    m[7] = 0.0f,
    m[8] = 10.0f * distort_hi[3];    m[9] = 10.0f * distort_mid[3];   m[10]= 10.0f * distort_lo[3];    m[11]= 0.0f;
    m[12]= 0.1f  * slope_distort_hi; m[13]= 0.1f * slope_distort_mid; m[14]= 0.1f  * slope_distort_lo; m[15]= 0.0f;
}

}