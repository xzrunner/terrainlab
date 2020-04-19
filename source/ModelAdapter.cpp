#include "terrainlab/ModelAdapter.h"

#include <heightfield/HeightField.h>
#include <node0/SceneNode.h>
#include <node0/CompMaterial.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompAABB.h>
#include <node3/CompModel.h>
#include <unirender2/Device.h>
#include <unirender2/IndexBuffer.h>
#include <unirender2/VertexBuffer.h>
#include <unirender2/VertexArray.h>
#include <unirender2/ComponentDataType.h>
#include <unirender2/VertexBufferAttribute.h>
#include <painting3/MaterialMgr.h>
#include <model/Model.h>
#include <model/typedef.h>
#include <terraingraph/HeightFieldEval.h>

namespace terrainlab
{

// todo: copy from sop::GeoAdaptor::Init
void ModelAdapter::SetupModel(const ur2::Device& dev, n0::SceneNode& node)
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

    auto model = std::make_shared<model::Model>(&dev);
    cmodel.SetModel(model);

    cmodel_inst.SetModel(model, 0);

    auto& caabb = node.HasUniqueComp<n3::CompAABB>() ?
        node.GetUniqueComp<n3::CompAABB>() : node.AddUniqueComp<n3::CompAABB>();
    caabb.DisableSerialize();
    caabb.SetAABB(model->aabb);
}

void ModelAdapter::UpdateModel(const ur2::Device& dev,
                               const hf::HeightField& hf,
                               const n0::SceneNode& node)
{
    auto model = std::make_shared<model::Model>(&dev);
    model->materials.emplace_back(std::make_unique<model::Model::Material>());
    model->meshes.push_back(HeightFieldToMesh(dev, hf));

    node.GetSharedComp<n3::CompModel>().SetModel(model);
    node.GetUniqueComp<n3::CompModelInst>().SetModel(model, 0);
}

std::unique_ptr<model::Model::Mesh>
ModelAdapter::HeightFieldToMesh(const ur2::Device& dev, const hf::HeightField& hf)
{
    const float tot_w = 1.0f;
    const float tot_h = 1.0f;

    auto mesh = std::make_unique<model::Model::Mesh>();

    const size_t w = hf.Width();
    const size_t h = hf.Height();
    std::vector<float> vertices;
    vertices.reserve(w * h * 8);
    sm::vec3 scale(tot_w / w, 0.001f, tot_h / h);
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            auto pos = sm::vec3(x, hf.Get(dev, x, y), y) * scale;
            vertices.insert(vertices.end(), pos.xyz, pos.xyz + 3);

            auto norm = terraingraph::HeightFieldEval::Normal(dev, hf, x, y, scale);
            vertices.insert(vertices.end(), norm.xyz, norm.xyz + 3);

            // texcoords
            vertices.push_back(static_cast<float>(x) / w);
            vertices.push_back(static_cast<float>(y) / h);
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

    auto va = dev.CreateVertexArray();

    auto usage = ur2::BufferUsageHint::StaticDraw;

    auto ibuf_sz = sizeof(unsigned short) * indices.size();
    auto ibuf = dev.CreateIndexBuffer(usage, ibuf_sz);
    ibuf->ReadFromMemory(indices.data(), ibuf_sz, 0);
    va->SetIndexBuffer(ibuf);

    auto vbuf_sz = sizeof(float) * vertices.size();
    auto vbuf = dev.CreateVertexBuffer(ur2::BufferUsageHint::StaticDraw, vbuf_sz);
    vbuf->ReadFromMemory(vertices.data(), vbuf_sz, 0);
    va->SetVertexBuffer(vbuf);

    std::vector<std::shared_ptr<ur2::VertexBufferAttribute>> vbuf_attrs(3);
    // pos
    vbuf_attrs[0] = std::make_shared<ur2::VertexBufferAttribute>(
        ur2::ComponentDataType::Float, 3, 0, 32
    );
    // normal
    vbuf_attrs[1] = std::make_shared<ur2::VertexBufferAttribute>(
        ur2::ComponentDataType::Float, 3, 12, 32
    );
    // texcoord
    vbuf_attrs[2] = std::make_shared<ur2::VertexBufferAttribute>(
        ur2::ComponentDataType::Float, 3, 24, 32
    );
    va->SetVertexBufferAttrs(vbuf_attrs);

    mesh->geometry.vertex_array = va;
	mesh->geometry.sub_geometries.push_back(model::SubmeshGeometry(true, indices.size(), 0));
	mesh->geometry.sub_geometry_materials.push_back(0);
	mesh->geometry.vertex_type |= model::VERTEX_FLAG_NORMALS;
	mesh->geometry.vertex_type |= model::VERTEX_FLAG_TEXCOORDS;
	mesh->material = 0;

    return mesh;
}

}