#pragma once

#include <model/Model.h>

namespace n0 { class SceneNode; }
namespace terraingraph { class HeightField; }

namespace terrainlab
{

class ModelAdapter
{
public:
    static void SetupModel(n0::SceneNode& node);

    static void UpdateModel(const terraingraph::HeightField& hf,
        const n0::SceneNode& node);

private:
    static std::unique_ptr<model::Model::Mesh>
        HeightFieldToMesh(const terraingraph::HeightField& hf);

}; // ModelAdapter

}