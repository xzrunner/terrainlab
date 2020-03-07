#include "terrainlab/ClipmapRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <renderpipeline/UniformNames.h>
#include <painting3/Shader.h>
#include <terraintiler/Clipmap.h>

namespace
{

const char* vs = R"(

attribute vec4 position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform vec4 u_block_ori;

uniform sampler2D u_heightmap;

varying vec3 v_fragpos;

void main()
{
    const float h_scale = 0.2;

    vec2 uv = position.xy * u_block_ori.xy + u_block_ori.zw;

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

    vec3 light_dir = normalize(vec3(0, -10, 10) - v_fragpos);
    float diff = max(dot(N, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
	gl_FragColor = vec4(diffuse, 1.0);

//	gl_FragColor = vec4(0, 0, 0, 1.0);
}

)";

}

namespace terrainlab
{

ClipmapRenderer::ClipmapRenderer()
{
    InitShader();
}

void ClipmapRenderer::Setup(std::shared_ptr<pt3::WindowContext>& wc) const
{
    static_cast<pt3::Shader*>(m_shader.get())->AddNotify(wc);
}

void ClipmapRenderer::Draw(const sm::mat4& mt) const
{
    if (!m_clipmap) {
        m_clipmap = std::make_shared<terraintiler::Clipmap>();
    }

    m_shader->Use();

    static_cast<pt0::Shader*>(m_shader.get())->UpdateModelMat(mt);

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    rc.SetPolygonMode(ur::POLYGON_LINE);
    size_t idx = 0;
    for (auto& layer : m_clipmap->GetAllLayers()) 
    {
        if (layer.heightmap) {
            rc.SetPolygonMode(ur::POLYGON_FILL);
            rc.BindTexture(layer.heightmap->TexID(), 0);
        } else {
            rc.SetPolygonMode(ur::POLYGON_LINE);
            rc.BindTexture(0, 0);
        }

        for (auto& block : layer.blocks)
        {
            m_shader->SetVec4("u_block_ori", block.trans.xyzw);

            rc.BindBuffer(ur::INDEXBUFFER, block.rd.ebo);
            rc.BindBuffer(ur::VERTEXBUFFER, block.rd.vbo);

            rc.DrawElements(ur::DRAW_TRIANGLES, 0, block.rd.num);
        }

        ++idx;
    }
    rc.SetPolygonMode(ur::POLYGON_FILL);
}

void ClipmapRenderer::InitShader()
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<ur::VertexAttrib> layout;
    layout.push_back(ur::VertexAttrib(rp::VERT_POSITION_NAME, 2, 4, 8, 0));
    rc.CreateVertexLayout(layout);

    std::vector<std::string> texture_names;
    texture_names.push_back("u_heightmap");

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