#include "terrainlab/SplatPbrRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/VertexAttrib.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ShaderUniforms.h>
#include <painting3/Shader.h>
#include <terraingraph/HeightField.h>
#include <terraingraph/TextureBaker.h>
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

	v_height = texture2D(u_heightmap, texcoord).r;

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

uniform sampler2D u_normal_map;
uniform sampler2D u_ao_map;

uniform sampler2D u_splatmap0;
uniform sampler2D u_splatmap1;
uniform sampler2D u_splatmap2;
uniform sampler2D u_splatmap3;

uniform vec3 u_cam_pos;

varying float v_height;
varying vec2  v_texcoord;
varying vec3  v_fragpos;
varying vec3  v_normal;

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

vec4 TexSampleNoTile(sampler2D samp, in vec3 world_pos, in vec3 weights)
{
    const float uv_scale = 16;
    vec3 uv = uv_scale * world_pos;
	vec3 blended = weights.xxx * textureNoTile2(samp, uv.yz).rgb +
	               weights.yyy * textureNoTile2(samp, uv.zx).rgb +
	               weights.zzz * textureNoTile2(samp, uv.xy).rgb;
    return vec4(blended, 1.0);
}

vec4 TexSample(sampler2D samp, in vec3 world_pos, in vec3 weights)
{
    const float uv_scale = 16;
    vec3 uv = uv_scale * world_pos;
	vec3 blended = weights.xxx * texture(samp, uv.yz).rgb +
	               weights.yyy * texture(samp, uv.zx).rgb +
	               weights.zzz * texture(samp, uv.xy).rgb;
    return vec4(blended, 1.0);
}

// PBR

const float PI = 3.14159265359;
//// ----------------------------------------------------------------------------
//// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
//// Don't worry if you don't get what's going on; you generally want to do normal
//// mapping the usual way for performance anways; I do plan make a note of this
//// technique somewhere later in the normal mapping tutorial.
//vec3 getNormalFromMap()
//{
//    vec3 tangentNormal = texture(u_normal_map, v_texcoord).xyz * 2.0 - 1.0;
//
//    vec3 Q1  = dFdx(WorldPos);
//    vec3 Q2  = dFdy(WorldPos);
//    vec2 st1 = dFdx(v_texcoord);
//    vec2 st2 = dFdy(v_texcoord);
//
//    vec3 N   = normalize(Normal);
//    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
//    vec3 B  = -normalize(cross(N, T));
//    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}



void main()
{
    // normal
    vec3 fdx = dFdx(v_fragpos);
    vec3 fdy = dFdy(v_fragpos);
//    vec3 N = normalize(cross(fdx, fdy));
    vec3 N = texture(u_normal_map, v_texcoord).rgb;

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

    vec3 env_col = mix(sky_col * sky_overbright, sun_col * sub_overbright, n_dot_l).rgb;
//    vec3 color = env_col * splat_col;

//    gl_FragColor = vec4(color, 1.0);





//    vec3 albedo     = pow(splat_col, vec3(2.2));
    vec3 albedo     = splat_col;
    float metallic  = 0.5;
    float roughness = 0.5;
    float ao        = texture(u_ao_map, v_texcoord).r;

    vec3 V = normalize(u_cam_pos - v_fragpos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    const vec3 light_pos = vec3(-0.5, 1.5, 1.0);
    const vec3 light_col = vec3(40, 40, 40);

    vec3 L = normalize(light_pos - v_fragpos);
    vec3 H = normalize(V + L);
    float distance = length(light_pos - v_fragpos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light_col * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular = nominator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
//    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 ambient = env_col * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
 //   color = color / (color + vec3(1.0));
    // gamma correct
//    color = pow(color, vec3(1.0/2.2));

    gl_FragColor = vec4(color, 1.0);
}

)";

}

namespace terrainlab
{

SplatPbrRenderer::SplatPbrRenderer()
{
    InitTextuers();
    InitShader();
}

void SplatPbrRenderer::Setup(const std::shared_ptr<terraingraph::HeightField>& hf)
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

    m_normal_map = terraingraph::TextureBaker::GenNormalMap(*hf, rc);
    m_ao_map = terraingraph::TextureBaker::GenAmbientOcclusionMap(*hf, rc);

    // textures
    if (m_height_map != old)
    {
        std::vector<uint32_t> texture_ids;
        texture_ids.reserve(6);
        texture_ids.push_back(m_height_map->TexID());
        texture_ids.push_back(m_normal_map->TexID());
        texture_ids.push_back(m_ao_map->TexID());
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

void SplatPbrRenderer::Clear()
{
    m_height_map.reset();
}

void SplatPbrRenderer::InitTextuers()
{
    m_splat_map[0] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\dark_dirt.jpg");
    m_splat_map[1] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\rock.jpg");
    m_splat_map[2] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\grass.jpg");
    m_splat_map[3] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\scape\\Terrain\\snow.png");
}

void SplatPbrRenderer::InitShader()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<ur::VertexAttrib> layout;
    layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    rc.CreateVertexLayout(layout);

    std::vector<std::string> texture_names;
    texture_names.push_back("u_heightmap");
    texture_names.push_back("u_normal_map");
    texture_names.push_back("u_ao_map");
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
    sp.uniform_names.Add(pt0::UniformTypes::CamPos,   "u_cam_pos");

    auto shader = std::make_shared<pt3::Shader>(&rc, sp);
    m_shaders.push_back(shader);
}

}