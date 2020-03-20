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

void Clipmap2dRenderer::Draw(float scale, const sm::vec2& offset,
                             float screen_width, float screen_height) const
{
    if (!m_tex)
    {
        textile::VTexInfo info;
        std::fstream fin(VTEX_FILEPATH, std::ios::in | std::ios::binary);
        textile::TileDataFile::ReadHeader(info, fin);
        fin.close();

        m_tex = std::make_shared<clipmap::Clipmap>(VTEX_FILEPATH, info);
    }

    if (m_tex) {
        m_tex->Update(scale, offset);
        m_tex->Draw(screen_width, screen_height);
    }
}

}