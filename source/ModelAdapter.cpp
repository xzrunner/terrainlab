#include "terrview/ModelAdapter.h"

#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompAABB.h>
#include <node3/CompModel.h>
#include <painting3/MaterialMgr.h>
#include <model/Model.h>
#include <model/typedef.h>
#include <terr/HeightField.h>
#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>

namespace terrv
{

// todo: copy from sop::GeoAdaptor::Init
void ModelAdapter::SetupModel(n0::SceneNode& node)
{
    auto& cmodel = node.AddSharedComp<n3::CompModel>();
    //cmodel.DisableSerialize();
    auto& cmodel_inst = node.AddUniqueComp<n3::CompModelInst>();
    //cmodel_inst.DisableSerialize();

    auto& cmaterial = node.AddUniqueComp<n0::CompMaterial>();
    cmaterial.DisableSerialize();
    auto mat = std::make_unique<pt0::Material>();
    typedef pt3::MaterialMgr::PhongUniforms UNIFORMS;
    mat->AddVar(UNIFORMS::ambient.name, pt0::RenderVariant(sm::vec3(0.04f, 0.04f, 0.04f)));
    mat->AddVar(UNIFORMS::diffuse.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
    mat->AddVar(UNIFORMS::specular.name, pt0::RenderVariant(sm::vec3(1, 1, 1)));
    mat->AddVar(UNIFORMS::shininess.name, pt0::RenderVariant(50.0f));
    cmaterial.SetMaterial(mat);

    auto model = std::make_shared<model::Model>();
    cmodel.SetModel(model);

    cmodel_inst.SetModel(model, 0);

    auto& caabb = node.HasUniqueComp<n3::CompAABB>() ?
        node.GetUniqueComp<n3::CompAABB>() : node.AddUniqueComp<n3::CompAABB>();
    caabb.DisableSerialize();
    caabb.SetAABB(model->aabb);
}

void ModelAdapter::UpdateModel(const terr::HeightField& hf,
                               const n0::SceneNode& node)
{
    auto model = std::make_shared<model::Model>();
    model->materials.emplace_back(std::make_unique<model::Model::Material>());
    model->meshes.push_back(HeightFieldToMesh(hf));

    node.GetSharedComp<n3::CompModel>().SetModel(model);
    node.GetUniqueComp<n3::CompModelInst>().SetModel(model, 0);
}

std::unique_ptr<model::Model::Mesh>
ModelAdapter::HeightFieldToMesh(const terr::HeightField& hf)
{
    const float tot_w = 1.0f;
    const float tot_h = 1.0f;

    auto mesh = std::make_unique<model::Model::Mesh>();

    const size_t w = hf.Width();
    const size_t h = hf.Height();
    std::vector<float> vertices;
    vertices.reserve(w * h * 3);
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            const float vx = tot_w / w * x;
            const float vy = tot_h / h * y;
            vertices.push_back(vx);
            vertices.push_back(hf.Get(x, y) * 0.001f);
            vertices.push_back(vy);
        }
    }

    std::vector<unsigned short> indices;
    indices.reserve((w - 1) * (h - 1) * 2);
    for (size_t y = 0; y < h - 1; ++y) {
        for (size_t x = 0; x < w - 1; ++x) {
            indices.push_back(y * w + x);
            indices.push_back((y + 1) * w + x + 1);
            indices.push_back(y * w + x + 1);
            indices.push_back(y * w + x);
            indices.push_back((y + 1) * w + x);
            indices.push_back((y + 1) * w + x + 1);
        }
    }

    ur::RenderContext::VertexInfo vi;

    const size_t stride = 3;    // float number
	vi.vn = w * h;
	vi.vertices = &vertices[0];
	vi.stride = stride * sizeof(float);
    vi.indices = indices.data();
    vi.in = indices.size();

	vi.va_list.push_back(ur::VertexAttrib("pos", 3, 4, 12, 0));   // vertices

	ur::Blackboard::Instance()->GetRenderContext().CreateVAO(
		vi, mesh->geometry.vao, mesh->geometry.vbo, mesh->geometry.ebo);
	mesh->geometry.sub_geometries.push_back(model::SubmeshGeometry(true, vi.in, 0));
	mesh->geometry.sub_geometry_materials.push_back(0);
	mesh->geometry.vertex_type |= model::VERTEX_FLAG_NORMALS;
	mesh->geometry.vertex_type |= model::VERTEX_FLAG_TEXCOORDS;
	mesh->material = 0;

    return mesh;
}

}