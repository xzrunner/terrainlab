#include "terrainlab/ImageRenderer.h"

#include <terraingraph/TextureBaker.h>
#include <unirender/Factory.h>
#include <unirender/Context.h>
#include <unirender/RenderState.h>
#include <painting2/RenderSystem.h>
#include <renderpipeline/RenderMgr.h>

namespace terrainlab
{

void ImageRenderer::Setup(const ur::Device& dev, const terraingraph::BitmapPtr& bmp)
{
    if (bmp) {
        m_tex = terraingraph::TextureBaker::GenColorMap(*bmp, dev);
    }
}

void ImageRenderer::Setup(const ur::Device& dev, const std::shared_ptr<terraingraph::Mask>& mask)
{
    if (mask) {
        m_tex = terraingraph::TextureBaker::GenColorMap(*mask, dev);
    }
}

void ImageRenderer::Draw(const ur::Device& dev, ur::Context& ctx) const
{
    if (!m_tex) {
        return;
    }

    auto rs = ur::DefaultRenderState2D();
    pt2::RenderSystem::DrawTexture(dev, ctx, rs, m_tex, sm::rect(512, 512), sm::Matrix2D(), false);
}

void ImageRenderer::Clear()
{
    m_tex.reset();
}

}