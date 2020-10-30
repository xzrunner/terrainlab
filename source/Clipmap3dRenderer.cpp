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

#version 330 core

layout (location = 0) in vec4 position;

layout(std140) uniform UBO_VS
{
    mat4 projection;
    mat4 view;
    mat4 model;

    vec4 block_ori;
    vec4 uv_region;
} ubo_vs;

uniform sampler2D u_heightmap;

out VS_OUT {
    vec3 frag_pos;
} vs_out;

void main()
{
    const float h_scale = 0.8;

    //vec2 uv = position.xy * 0.5 + 0.5;
    vec2 uv = position.xy;
    uv = uv * ubo_vs.block_ori.xy + ubo_vs.block_ori.zw;
    uv = uv * ubo_vs.uv_region.zw + ubo_vs.uv_region.xy;

    vec4 pos;
    pos.xz = position.xy;
    pos.y = texture(u_heightmap, uv).r * h_scale;
    pos.w = 1;

    vs_out.frag_pos = vec3(ubo_vs.model * pos);
	gl_Position = ubo_vs.projection * ubo_vs.view * ubo_vs.model * pos;
}

)";

const char* fs = R"(

#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 frag_pos;
} fs_in;

void main()
{
    vec3 fdx = dFdx(fs_in.frag_pos);
    vec3 fdy = dFdy(fs_in.frag_pos);
    vec3 N = normalize(cross(fdx, fdy));

    vec3 light_dir = normalize(vec3(0, -100000, 100000) - fs_in.frag_pos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	FragColor = vec4(diffuse, 1.0);

//	FragColor = vec4(0, 0, 0, 1.0);
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
    assert(m_shader);

    m_shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*m_shader, "ubo_vs.model"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*m_shader, "ubo_vs.view"));
    m_shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*m_shader, "ubo_vs.projection"));
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

        auto uv_region = m_shader->QueryUniform("ubo_vs.uv_region");
        uv_region->SetValue(layers[i].uv_region.xyzw, 4);
        for (auto& block : layers[i].blocks)
        {
            auto block_ori = m_shader->QueryUniform("ubo_vs.block_ori");
            block_ori->SetValue(block.trans.xyzw, 4);

            draw.vertex_array = block.rd.va;
            ctx.Draw(ur::PrimitiveType::Triangles, draw, nullptr);
        }
    }
}

}