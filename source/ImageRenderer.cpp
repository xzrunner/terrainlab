#include "terrainlab/ImageRenderer.h"

#include <terraingraph/TextureBaker.h>
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

    ur2::RenderState rs;
    rs.depth_test.enabled = false;
    rs.facet_culling.enabled = false;

    rs.blending.enabled = true;
    rs.blending.separately = false;
    rs.blending.src = ur2::BlendingFactor::One;
    rs.blending.dst = ur2::BlendingFactor::OneMinusSrcAlpha;
    rs.blending.equation = ur2::BlendEquation::Add;

    pt2::RenderSystem::DrawTexture(dev, ctx, rs, m_tex, sm::rect(512, 512), sm::Matrix2D(), false);
}

void ImageRenderer::Clear()
{
    m_tex.reset();
}

}