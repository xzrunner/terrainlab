#pragma once

#include <model/Model.h>

namespace n0 { class SceneNode; }
namespace hf { class HeightField; }

namespace terrainlab
{

class ModelAdapter
{
public:
    static void SetupModel(n0::SceneNode& node);

    static void UpdateModel(const hf::HeightField& hf,
        const n0::SceneNode& node);

private:
    static std::unique_ptr<model::Model::Mesh>
        HeightFieldToMesh(const hf::HeightField& hf);

}; // ModelAdapter

}