#include "terrainlab/FullView3dRenderer.h"

#include <SM_Calc.h>
#include <unirender/Device.h>
#include <unirender/ShaderProgram.h>
#include <unirender/DrawState.h>
#include <unirender/Context.h>
#include <shadertrans/ShaderTrans.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/Shader.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <terraintiler/GeoMipMapping.h>

namespace
{

const char* vs = R"(

#version 330 core

layout (location = 0) in vec4 position;

layout(std140) uniform UBO_VS
{
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo_vs;

out VS_OUT {
    vec3 frag_pos;
} vs_out;

void main()
{
    vs_out.frag_pos = vec3(ubo_vs.model * position);
	gl_Position = ubo_vs.projection * ubo_vs.view * ubo_vs.model * position;
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

    vec3 light_dir = normalize(vec3(0, -10, 10) - fs_in.frag_pos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	FragColor = vec4(diffuse, 1.0);
}

)";

}

namespace terrainlab
{

FullView3dRenderer::FullView3dRenderer(const ur::Device& dev)
{
    InitShader(dev);

    m_mipmap = std::make_shared<terraintiler::GeoMipMapping>(5, 5);
}

void FullView3dRenderer::Setup(std::shared_ptr<pt3::WindowContext>& wc) const
{
//    static_cast<pt3::Shader*>(m_shader.get())->AddNotify(wc);
}

void FullView3dRenderer::Update(const ur::Device& dev)
{
    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            m_mipmap->UpdateTile(dev, x, y);
        }
    }
}

void FullView3dRenderer::Draw(ur::Context& ctx, const sm::vec3& cam_pos,
                              const sm::mat4& mt, bool debug_draw) const
{
//    m_shader->Bind();

    auto model_updater = m_shader->QueryUniformUpdater(ur::GetUpdaterTypeID<pt0::ModelMatUpdater>());
    if (model_updater) {
        std::static_pointer_cast<pt0::ModelMatUpdater>(model_updater)->Update(mt);
    }

    ur::DrawState ds;
    ds.program = m_shader;

    if (debug_draw) {
        ds.render_state.rasterization_mode = ur::RasterizationMode::Line;
    }

    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            const float dist = sm::dis_pos3_to_pos3(cam_pos, sm::vec3(static_cast<float>(x), 0, static_cast<float>(y)));
            size_t lod_level = 5;
            if (dist < 2) {
                lod_level = 0;
            } else if (dist < 4) {
                lod_level = 1;
            } else if (dist < 8) {
                lod_level = 2;
            } else if (dist < 16) {
                lod_level = 3;
            } else if (dist < 32) {
                lod_level = 4;
            } else if (dist < 64) {
                lod_level = 5;
            }

            auto rd = m_mipmap->QueryRenderable(x, y, lod_level);
            if (!rd.va) {
                continue;
            }

            ds.vertex_array = rd.va;
            ctx.Draw(ur::PrimitiveType::Triangles, ds, nullptr);
        }
    }
}

void FullView3dRenderer::InitShader(const ur::Device& dev)
{
    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 12, 0));
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

}