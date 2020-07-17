#include "terrainlab/Clipmap3dRenderer.h"

#include <unirender/ShaderProgram.h>
#include <unirender/UniformUpdater.h>
#include <unirender/Uniform.h>
#include <unirender/Context.h>
#include <unirender/DrawState.h>
#include <unirender/Device.h>
#include <shadertrans/ShaderTrans.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/Shader.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <terraintiler/Clipmap.h>
#include <clipmap/TextureStack.h>
#include <clipmap/Clipmap.h>

namespace
{

const char* VTEX_FILEPATH = "D:\\OneDrive\\asset\\terrain\\gebco_08_rev_elev_21600x10800.vtex";

const char* vs = R"(

attribute vec4 position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform vec4 u_block_ori;
uniform vec4 u_uv_region;

uniform sampler2D u_heightmap;

varying vec3 v_fragpos;

void main()
{
    const float h_scale = 0.8;

    //vec2 uv = position.xy * 0.5 + 0.5;
    vec2 uv = position.xy;
    uv = uv * u_block_ori.xy + u_block_ori.zw;
    uv = uv * u_uv_region.zw + u_uv_region.xy;

    vec4 pos;
    pos.xz = position.xy;
    pos.y = texture2D(u_heightmap, uv).r * h_scale;
    pos.w = 1;

    v_fragpos = vec3(u_model * pos);
	gl_Position = u_projection * u_view * u_model * pos;
}

)";

const char* fs = R"(

varying vec3 v_fragpos;

void main()
{
    vec3 fdx = dFdx(v_fragpos);
    vec3 fdy = dFdy(v_fragpos);
    vec3 N = normalize(cross(fdx, fdy));

    vec3 light_dir = normalize(vec3(0, -100000, 100000) - v_fragpos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	gl_FragColor = vec4(diffuse, 1.0);

//	gl_FragColor = vec4(0, 0, 0, 1.0);
}

)";

}

namespace terrainlab
{

Clipmap3dRenderer::Clipmap3dRenderer(const ur::Device& dev)
{
    InitShader(dev);
}

void Clipmap3dRenderer::Setup(std::shared_ptr<pt3::WindowContext>& wc) const
{
//    static_cast<pt3::Shader*>(m_shader.get())->AddNotify(wc);
}

void Clipmap3dRenderer::Draw(const ur::Device& dev, ur::Context& ctx, const sm::mat4& mt) const
{
    if (!m_vtex) {
        m_vtex = std::make_shared<terraintiler::Clipmap>(dev, VTEX_FILEPATH);
    }

//    m_shader->Bind();

    auto model_updater = m_shader->QueryUniformUpdater(ur::GetUpdaterTypeID<pt0::ModelMatUpdater>());
    if (model_updater) {
        std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater)->Update(mt);
    }

    auto& layers = m_vtex->GetAllLayers();
    float scale;
    sm::vec2 offset;
    m_vtex->GetVTex()->GetRegion(scale, offset);
    auto level = clipmap::TextureStack::CalcMipmapLevel(layers.size(), scale);
    DrawLayer(ctx, level);

    m_vtex->DebugDraw(dev, ctx);
}

//std::shared_ptr<clipmap::Clipmap>
//Clipmap3dRenderer::GetVTex() const
//{
//    return m_vtex ? m_vtex->GetVTex() : nullptr;
//}

void Clipmap3dRenderer::InitShader(const ur::Device& dev)
{
    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 2, 4, 8, 0));
    //rc.CreateVertexLayout(layout);

    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs, _fs);
    m_shader = dev.CreateShaderProgram(_vs, _fs);

    m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*m_shader, rp::MODEL_MAT_NAME));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*m_shader, rp::VIEW_MAT_NAME));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*m_shader, rp::PROJ_MAT_NAME));
}

void Clipmap3dRenderer::DrawLayer(ur::Context& ctx, size_t start_level) const
{
    auto heightmap_slot = m_shader->QueryTexSlot("u_heightmap");

    ur::DrawState draw;
    draw.program = m_shader;

    auto& layers = m_vtex->GetAllLayers();
    for (size_t i = 0, n = 2/*layers.size()*/; i < n; ++i)
    {
        if (layers[i].heightmap) {
            //rc.SetPolygonMode(ur::POLYGON_FILL);
            ctx.SetTexture(heightmap_slot, layers[std::max(i, start_level)].heightmap);
        } else {
            //rc.SetPolygonMode(ur::POLYGON_LINE);
            ctx.SetTexture(heightmap_slot, nullptr);
            return;
        }

        auto uv_region = m_shader->QueryUniform("u_uv_region");
        uv_region->SetValue(layers[i].uv_region.xyzw, 4);
        for (auto& block : layers[i].blocks)
        {
            auto block_ori = m_shader->QueryUniform("u_block_ori");
            block_ori->SetValue(block.trans.xyzw, 4);

            draw.vertex_array = block.rd.va;
            ctx.Draw(ur::PrimitiveType::Triangles, draw, nullptr);
        }
    }
}

}