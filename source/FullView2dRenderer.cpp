#include "terrainlab/FullView2dRenderer.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
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

void FullView2dRenderer::Draw() const
{
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    rc.SetZTest(ur::DEPTH_DISABLE);
    rc.SetCullMode(ur::CULL_DISABLE);

    auto w = m_mipmap->GetWidth();
    auto h = m_mipmap->GetHeight();
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            auto hmap = m_mipmap->QueryHeightmap(x, y);

            sm::Matrix2D mt;
            mt.Translate((TILE_SIZE + TILE_GAP) * x, (TILE_SIZE + TILE_GAP) * y);

            pt2::RenderSystem::DrawTexture(
                *hmap, sm::rect(TILE_SIZE, TILE_SIZE), mt, false
            );
        }
    }
}

void FullView2dRenderer::Clear()
{
}

}