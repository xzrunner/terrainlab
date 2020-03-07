#include "terrainlab/FullView3dRenderer.h"

#include <SM_Calc.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting3/Shader.h>
#include <terraintiler/GeoMipMapping.h>

namespace
{

const char* vs = R"(

attribute vec4 position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

varying vec3 v_fragpos;

void main()
{
    v_fragpos = vec3(u_model * position);
	gl_Position = u_projection * u_view * u_model * position;
}

)";

const char* fs = R"(

varying vec3 v_fragpos;

void main()
{
    vec3 fdx = dFdx(v_fragpos);
    vec3 fdy = dFdy(v_fragpos);
    vec3 N = normalize(cross(fdx, fdy));

    vec3 light_dir = normalize(vec3(0, -10, 10) - v_fragpos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	gl_FragColor = vec4(diffuse, 1.0);
}

)";

}

namespace terrainlab
{

FullView3dRenderer::FullView3dRenderer()
{
    InitShader();

    m_mipmap = std::make_shared<terraintiler::GeoMipMapping>(5, 5);
}

void FullView3dRenderer::Setup(std::shared_ptr<pt3::WindowContext>& wc) const
{
    static_cast<pt3::Shader*>(m_shader.get())->AddNotify(wc);
}

void FullView3dRenderer::Update()
{
    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            m_mipmap->UpdateTile(x, y);
        }
    }   
}

void FullView3dRenderer::Draw(const sm::vec3& cam_pos,
                              const sm::mat4& mt, bool debug_draw) const
{
    m_shader->Use();

    static_cast<pt0::Shader*>(m_shader.get())->UpdateModelMat(mt);

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    if (debug_draw) {
        rc.SetPolygonMode(ur::POLYGON_LINE);
    }

    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            const float dist = sm::dis_pos3_to_pos3(cam_pos, sm::vec3(x, 0, y));
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
            if (rd.num == 0) {
                continue;
            }

            rc.BindBuffer(ur::INDEXBUFFER, rd.ebo);
            rc.BindBuffer(ur::VERTEXBUFFER, rd.vbo);

            rc.DrawElements(ur::DRAW_TRIANGLES, 0, rd.num, false);
        }
    }    

    if (debug_draw) {
        rc.SetPolygonMode(ur::POLYGON_FILL);
    }
}

void FullView3dRenderer::InitShader()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<ur::VertexAttrib> layout;
    layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 3, 4, 12, 0));
    rc.CreateVertexLayout(layout);

    std::vector<std::string> texture_names;

    pt3::Shader::Params sp(texture_names, layout);
    sp.vs = vs;
    sp.fs = fs;

    sp.uniform_names.Add(pt0::UniformTypes::ModelMat, rp::MODEL_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ViewMat,  rp::VIEW_MAT_NAME);
    sp.uniform_names.Add(pt0::UniformTypes::ProjMat,  rp::PROJ_MAT_NAME);
    //sp.uniform_names.Add(pt0::UniformTypes::CamPos, sw::node::CameraPos::CamPosName());

    m_shader = std::make_unique<pt3::Shader>(&rc, sp);
}

}