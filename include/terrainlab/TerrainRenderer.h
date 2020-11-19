#pragma once

#include <renderlab/Evaluator.h>

#include <painting0/Camera.h>

namespace ur { class Device; class Context; }
namespace renderlab { class Evaluator; }

namespace terrainlab
{

class TerrainRenderer
{
public:
	TerrainRenderer(const ur::Device& dev);

	void Draw(const ur::Device& dev, ur::Context& ctx, const pt0::CameraPtr& cam) const;

	void SetHeightmap(const ur::TexturePtr& tex);

private:
	renderlab::Evaluator m_hf_rd;

}; // TerrainRenderer

}