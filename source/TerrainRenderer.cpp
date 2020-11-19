#include "terrainlab/TerrainRenderer.h"

#include <renderlab/RenderAdapter.h>

#include <rendergraph/RenderContext.h>

namespace terrainlab
{

TerrainRenderer::TerrainRenderer(const ur::Device& dev)
{
	renderlab::RenderAdapter::BuildRenderer("assets/render/mountain.json",
		dev, m_hf_rd);
}

void TerrainRenderer::Draw(const ur::Device& dev, ur::Context& ctx, const pt0::CameraPtr& cam) const
{
	auto rc = std::make_shared<rendergraph::RenderContext>(nullptr);

	rc->ur_dev = &dev;
	rc->ur_ctx = &ctx;

	rc->cam_view_mat = cam->GetViewMat();
	rc->cam_proj_mat = cam->GetProjectionMat();

	m_hf_rd.Draw(rc);
}

void TerrainRenderer::SetHeightmap(const ur::TexturePtr& tex)
{
	m_hf_rd.SetInputVar("tex", tex);
}

}