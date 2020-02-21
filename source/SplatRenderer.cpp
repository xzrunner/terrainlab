#include "wmv/SplatRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/VertexAttrib.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting3/Shader.h>
#include <wm/HeightField.h>
#include <wm/TextureBaker.h>
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

varying float v_height;
varying vec2  v_texcoord;

void main()
{
    const float h_scale = 0.2;

	v_height = texture2D(u_heightmap, texcoord).a;

	vec4 pos = position;
	pos.y = v_height * h_scale;
	gl_Position = u_projection * u_view * u_model * pos;

	v_texcoord = texcoord;
}

)";

const char* fs = R"(

uniform sampler2D u_detailmap;

uniform sampler2D u_splatmap0;
uniform sampler2D u_splatmap1;
uniform sampler2D u_splatmap2;
uniform sampler2D u_splatmap3;

varying float v_height;
varying vec2  v_texcoord;

void main()
{
	vec4 splat_col;
	if (v_height < 0.33) {
		float t = v_height / 0.33;
		splat_col = texture2D(u_splatmap0, v_texcoord) * (1 - t) + texture2D(u_splatmap1, v_texcoord) * t;
	} else if (v_height < 0.66) {
		float t = (v_height - 0.33) / 0.33;
		splat_col = texture2D(u_splatmap1, v_texcoord) * (1 - t) + texture2D(u_splatmap2, v_texcoord) * t;
	} else {
		float t = (v_height - 0.66) / 0.33;
		splat_col = texture2D(u_splatmap2, v_texcoord) * (1 - t) + texture2D(u_splatmap3, v_texcoord) * t;
	}

	vec4 detail_col = texture2D(u_detailmap, v_texcoord);
	gl_FragColor = splat_col * detail_col;
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
        texture_ids.push_back(m_detail_map->TexID());
        texture_ids.push_back(m_splat_map[0]->TexID());
        texture_ids.push_back(m_splat_map[1]->TexID());
        texture_ids.push_back(m_splat_map[2]->TexID());
        texture_ids.push_back(m_splat_map[3]->TexID());

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
    m_detail_map = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\detailMap.tga");

    m_splat_map[0] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\lowestTile.tga");
    m_splat_map[1] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\lowTile.tga");
    m_splat_map[2] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\highTile.tga");
    m_splat_map[3] = model::TextureLoader::LoadFromFile("D:\\OneDrive\\asset\\terrain\\highestTile.tga");
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
    texture_names.push_back("u_detailmap");
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