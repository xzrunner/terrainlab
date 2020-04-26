#include "terrainlab/ImageRenderer.h"

#include <terraingraph/TextureBaker.h>
#include <unirender2/Factory.h>
#include <unirender2/Context.h>
#include <unirender2/RenderState.h>
#include <painting2/RenderSystem.h>
#include <renderpipeline/RenderMgr.h>

namespace terrainlab
{

void ImageRenderer::Setup(const ur2::Device& dev, const terraingraph::BitmapPtr& bmp)
{
    if (bmp) {
        m_tex = terraingraph::TextureBaker::GenColorMap(*bmp, dev);
    }
}

void ImageRenderer::Setup(const ur2::Device& dev, const std::shared_ptr<terraingraph::Mask>& mask)
{
    if (mask) {
        m_tex = terraingraph::TextureBaker::GenColorMap(*mask, dev);
    }
}

void ImageRenderer::Draw(const ur2::Device& dev, ur2::Context& ctx) const
{
    if (!m_tex) {
        return;
    }

    auto rs = ur2::DefaultRenderState2D();
    pt2::RenderSystem::DrawTexture(dev, ctx, rs, m_tex, sm::rect(512, 512), sm::Matrix2D(), false);
}

void ImageRenderer::Clear()
{
    m_tex.reset();
}

}