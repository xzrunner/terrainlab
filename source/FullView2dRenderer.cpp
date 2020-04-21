#include "terrainlab/FullView2dRenderer.h"

#include <unirender2/RenderState.h>
#include <painting2/RenderSystem.h>
#include <terraintiler/GeoMipMapping.h>

#include <ee0/RenderContext.h>

namespace
{

const size_t TILE_SIZE = 128;
const size_t TILE_GAP  = 0;

}

namespace terrainlab
{

FullView2dRenderer::FullView2dRenderer()
{
    m_mipmap = std::make_shared<terraintiler::GeoMipMapping>(16, 16);
}

void FullView2dRenderer::Draw(const ur2::Device& dev, ur2::Context& ctx) const
{
    ur2::RenderState rs;
    rs.depth_test.enabled = false;
    rs.facet_culling.enabled = false;

    rs.blending.enabled = true;
    rs.blending.separately = false;
    rs.blending.src = ur2::BlendingFactor::One;
    rs.blending.dst = ur2::BlendingFactor::OneMinusSrcAlpha;
    rs.blending.equation = ur2::BlendEquation::Add;

    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            auto hmap = m_mipmap->QueryHeightmap(x, y);

            sm::Matrix2D mt;
            mt.Translate(static_cast<float>(TILE_SIZE + TILE_GAP) * x,
                static_cast<float>(TILE_SIZE + TILE_GAP) * y);

            pt2::RenderSystem::DrawTexture(
                dev, ctx, rs, hmap, sm::rect(TILE_SIZE, TILE_SIZE), mt, false
            );
        }
    }
}

void FullView2dRenderer::Clear()
{
}

}