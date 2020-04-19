#include "terrainlab/VTexRenderer.h"

#include <unirender2/RenderState.h>
#include <painting2/RenderSystem.h>
#include <terraintiler/VirtualTexture.h>
#include <vtex/VirtualTexture.h>

namespace
{

const char* VTEX_FILEPATH = "D:\\OneDrive\\asset\\terrain\\world.topo.bathy.200412.3x5400x2700.vtex";

}

namespace terrainlab
{

VTexRenderer::VTexRenderer()
{
}

void VTexRenderer::Draw(const ur2::Device& dev, ur2::Context& ctx,
                        const sm::mat4& mt) const
{
    if (!m_vtex) {
        m_vtex = std::make_shared<terraintiler::VirtualTexture>(dev, VTEX_FILEPATH);
    }

    auto vtex = m_vtex->GetVTex();
    auto draw_cb = [&]()
    {
        auto w = vtex->Width();
        auto h = vtex->Height();
        sm::rect pos(0, 0, 256, 256);

        ur2::RenderState rs;
        pt2::RenderSystem::DrawTexture(dev, ctx, rs, w, h, 0, pos, sm::Matrix2D(), false);
    };
    vtex->Draw(dev, ctx, draw_cb);
}

}