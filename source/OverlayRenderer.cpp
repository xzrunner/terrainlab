#include "terrview/OverlayRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/VertexAttrib.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting3/Shader.h>
#include <terr/HeightField.h>
#include <terr/TextureBaker.h>
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
	vec4 pos = position;
	pos.y = texture2D(u_heightmap, texcoord).a;
	gl_Position = u_projection * u_view * u_model * pos;

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_colormap;
uniform sampler2D u_detailmap;

varying vec2 v_texcoord;

void main()
{
    vec4 base_col = texture2D(u_colormap, v_texcoord);
	vec4 detail_col = texture2D(u_detailmap, v_texcoord);
	gl_FragColor = base_col * detail_col;
}

)";

}

namespace terrv
{

OverlayRenderer::OverlayRenderer()
{
    InitTextuers();
    InitShader();
}

void OverlayRenderer::Clear()
{
    m_height_map.reset();
    m_color_map.reset();
}

void OverlayRenderer::Setup(const std::shared_ptr<terr::HeightField>& hf,
                            const std::shared_ptr<terr::Bitmap>& bmp)
{
    m_hf = hf;
    if (!m_hf) {
        return;
    }

    if (!bmp) {
        return;
    }
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    m_color_map = terr::TextureBaker::GenColorMap(*bmp, rc);

    assert(hf);
    auto old = m_height_map;
    m_height_map = terr::TextureBaker::GenHeightMap(*hf, rc, m_height_map);

    // textures
    if (m_height_map != old)
    {
        std::vector<uint32_t> texture_ids;
        texture_ids.reserve(3);
        texture_ids.push_back(m_height_map->TexID());
        texture_ids.push_back(m_color_map->TexID());
        texture_ids.push_back(m_detail_map->TexID());

        assert(m_shaders.size() == 1);
        m_shaders.front()->SetUsedTextures(texture_ids);
    }

    // vertex buffer
    if (!old ||
        old->Width() != m_height_map->Width() ||
        old->Height() != m_height_map->Height()) {
        BuildVertBuf();
    }
}

void OverlayRenderer::Draw() const
{
    if (m_shaders.empty() || !m_hf) {
        return;
    }

    DrawVertBuf();
}

void OverlayRenderer::InitTextuers()
{
    m_detail_map = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\detailMap.tga");
}

void OverlayRenderer::InitShader()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<ur::VertexAttrib> layout;
    layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 20, 0));
    layout.push_back(ur::VertexAttrib(rp::VERT_TEXCOORD_NAME, 2, 4, 20, 12));
    rc.CreateVertexLayout(layout);

    std::vector<std::string> texture_names;
    texture_names.push_back("u_heightmap");
    texture_names.push_back("u_colormap");
    texture_names.push_back("u_detailmap");

    pt3::Shader::Params sp(texture_names, layout);
    sp.vs = vs;
    sp.fs = fs;

    sp.uniform_names.Add(pt0::UniformTypes::ModelMat, rp::MODEL_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  rp::VIEW_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  rp::PROJ_MAT_NAME);

    auto shader = std::make_shared<pt3::Shader>(&rc, sp);
    m_shaders.push_back(shader);
}

}