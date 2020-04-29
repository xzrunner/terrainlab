#include "terrainlab/Clipmap2dRenderer.h"

#include <clipmap/Clipmap.h>
#include <terraintiler/VirtualTexture.h>

namespace
{

//const char* VTEX_FILEPATH = "D:\\OneDrive\\asset\\terrain\\world.topo.bathy.200412.3x5400x2700.vtex";
const char* VTEX_FILEPATH = "D:\\OneDrive\\asset\\terrain\\gebco_08_rev_elev_21600x10800.vtex";

}

namespace terrainlab
{

void Clipmap2dRenderer::Draw(const ur::Device& dev, ur::Context& ctx,
                             float screen_width, float screen_height) const
{
    if (!m_vtex)
    {
        textile::VTexInfo info;
        std::fstream fin(VTEX_FILEPATH, std::ios::in | std::ios::binary);
        textile::TileDataFile::ReadHeader(info, fin);
        fin.close();

        m_vtex = std::make_shared<clipmap::Clipmap>(VTEX_FILEPATH, info);
        m_vtex->Update(dev, ctx, 1.0f, sm::vec2(0, 0));
    }

    if (m_vtex) {
        m_vtex->Draw(dev, ctx, screen_width, screen_height);
    }
}

}