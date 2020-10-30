#include "terrainlab/OverlayRenderer.h"

#include <heightfield/HeightField.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Texture.h>
#include <shadertrans/ShaderTrans.h>
#include <renderpipeline/UniformNames.h>
#include <painting0/ModelMatUpdater.h>
#include <painting3/Shader.h>
#include <painting3/ViewMatUpdater.h>
#include <painting3/ProjectMatUpdater.h>
#include <terraingraph/TextureBaker.h>
#include <model/TextureLoader.h>

namespace
{

const char* vs = R"(

#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

layout(std140) uniform UBO_VS
{
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo_vs;

uniform sampler2D u_heightmap;

out VS_OUT {
    vec2 texcoord;
} vs_out;

void main()
{
    const float h_scale = 0.2;
	vec4 pos = position;
	pos.y = texture(u_heightmap, texcoord).r * h_scale;
	gl_Position = ubo_vs.projection * ubo_vs.view * ubo_vs.model * pos;

	vs_out.texcoord = texcoord;
}

)";

const char* fs = R"(

#version 330 core

out vec4 FragColor;

uniform sampler2D u_colormap;

in VS_OUT {
    vec2 texcoord;
} fs_in;

void main()
{
    vec4 base_col = texture(u_colormap, fs_in.texcoord);
	FragColor = base_col;
}

)";

}

namespace terrainlab
{

OverlayRenderer::OverlayRenderer(const ur::Device& dev)
    : rp::HeightfieldRenderer(dev)
{
    InitShader(dev);
}

void OverlayRenderer::Clear()
{
    m_height_map.reset();
    m_color_map.reset();
}

void OverlayRenderer::Setup(const ur::Device& dev, ur::Context& ctx,
                            const std::shared_ptr<hf::HeightField>& hf,
                            const terraingraph::BitmapPtr& bmp)
{
    m_hf = hf;
    if (!m_hf) {
        return;
    }

    if (!bmp) {
        return;
    }
    m_color_map = terraingraph::TextureBaker::GenColorMap(*bmp, dev);

    assert(hf);
    auto old = m_height_map;
    m_height_map = hf->GetHeightmap(dev);

    // textures
    if (m_height_map != old)
    {
        assert(m_shaders.size() == 1);
        auto shader = m_shaders.front();
        ctx.SetTexture(shader->QueryTexSlot("u_heightmap"), m_height_map);
        ctx.SetTexture(shader->QueryTexSlot("u_colormap"), m_color_map);
    }

    // vertex buffer
    if (!old ||
        old->GetWidth() != m_height_map->GetWidth() ||
        old->GetHeight() != m_height_map->GetHeight()) {
        BuildVertBuf(ctx);
    }
}

void OverlayRenderer::InitShader(const ur::Device& dev)
{
    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    //layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    //rc.CreateVertexLayout(layout);

    std::vector<unsigned int> _vs, _fs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::VertexShader, vs, _vs);
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::PixelShader, fs, _fs);
    auto shader = dev.CreateShaderProgram(_vs, _fs);
    assert(shader);

    shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*shader, "ubo_vs.model"));
    shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*shader, "ubo_vs.view"));
    shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*shader, "ubo_vs.projection"));

    m_shaders.push_back(shader);
}

}