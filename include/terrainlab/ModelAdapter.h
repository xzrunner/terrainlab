#pragma once

#include <model/Model.h>

namespace n0 { class SceneNode; }
namespace hf { class HeightField; }
namespace ur { class Device; }

namespace terrainlab
{

class ModelAdapter
{
public:
    static void SetupModel(const ur::Device& dev, n0::SceneNode& node);

    static void UpdateModel(const ur::Device& dev,
        const hf::HeightField& hf, const n0::SceneNode& node);

private:
    static std::unique_ptr<model::Model::Mesh>
        HeightFieldToMesh(const ur::Device& dev, const hf::HeightField& hf);

}; // ModelAdapter

}