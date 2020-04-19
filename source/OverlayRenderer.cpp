#include "terrainlab/OverlayRenderer.h"

#include <heightfield/HeightField.h>
#include <unirender2/ShaderProgram.h>
#include <unirender2/Texture.h>
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

attribute vec4 position;
attribute vec2 texcoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform sampler2D u_heightmap;

varying vec2 v_texcoord;

void main()
{
    const float h_scale = 0.2;
	vec4 pos = position;
	pos.y = texture2D(u_heightmap, texcoord).r * h_scale;
	gl_Position = u_projection * u_view * u_model * pos;

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_colormap;

varying vec2 v_texcoord;

void main()
{
    vec4 base_col = texture2D(u_colormap, v_texcoord);
	gl_FragColor = base_col;
}

)";

}

namespace terrainlab
{

OverlayRenderer::OverlayRenderer(const ur2::Device& dev)
    : rp::HeightfieldRenderer(dev)
{
    InitShader(dev);
}

void OverlayRenderer::Clear()
{
    m_height_map.reset();
    m_color_map.reset();
}

void OverlayRenderer::Setup(const ur2::Device& dev, ur2::Context& ctx,
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

void OverlayRenderer::InitShader(const ur2::Device& dev)
{
    //std::vector<ur::VertexAttrib> layout;
    //layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    //layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    //rc.CreateVertexLayout(layout);

    auto shader = dev.CreateShaderProgram(vs, fs);
    shader->AddUniformUpdater(std::make_shared<pt0::ModelMatUpdater>(*shader, rp::MODEL_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ViewMatUpdater>(*shader, rp::VIEW_MAT_NAME));
    shader->AddUniformUpdater(std::make_shared<pt3::ProjectMatUpdater>(*shader, rp::PROJ_MAT_NAME));
    m_shaders.push_back(shader);
}

}