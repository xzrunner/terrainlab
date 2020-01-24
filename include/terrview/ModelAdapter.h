#pragma once

#include <model/Model.h>

namespace n0 { class SceneNode; }
namespace terr { class HeightField; }

namespace terrv
{

class ModelAdapter
{
public:
    static void SetupModel(n0::SceneNode& node);

    static void UpdateModel(const terr::HeightField& hf,
        const n0::SceneNode& node);

private:
    static std::unique_ptr<model::Model::Mesh>
        HeightFieldToMesh(const terr::HeightField& hf);

}; // ModelAdapter

}