#pragma once

#include <model/Model.h>

namespace n0 { class SceneNode; }
namespace wm { class HeightField; }

namespace wmv
{

class ModelAdapter
{
public:
    static void SetupModel(n0::SceneNode& node);

    static void UpdateModel(const wm::HeightField& hf,
        const n0::SceneNode& node);

private:
    static std::unique_ptr<model::Model::Mesh>
        HeightFieldToMesh(const wm::HeightField& hf);

}; // ModelAdapter

}