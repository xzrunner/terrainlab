#include "terrainlab/FullView2dRenderer.h"

#include <unirender/Factory.h>
#include <unirender/RenderState.h>
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

void FullView2dRenderer::Draw(const ur::Device& dev, ur::Context& ctx) const
{
    auto rs = ur::DefaultRenderState2D();
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