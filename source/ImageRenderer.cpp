#include "terrainlab/ImageRenderer.h"

#include <terraingraph/TextureBaker.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting2/RenderSystem.h>
#include <renderpipeline/RenderMgr.h>

namespace terrainlab
{

void ImageRenderer::Setup(const std::shared_ptr<terraingraph::Bitmap>& bmp)
{
    if (bmp) {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        m_tex = terraingraph::TextureBaker::GenColorMap(*bmp, rc);
    }
}

void ImageRenderer::Setup(const std::shared_ptr<terraingraph::Mask>& mask)
{
    if (mask) {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        m_tex = terraingraph::TextureBaker::GenColorMap(*mask, rc);
    }
}

void ImageRenderer::Draw() const
{
    if (!m_tex) {
        return;
    }

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    rc.SetZTest(ur::DEPTH_DISABLE);

    pt2::RenderSystem::DrawTexture(*m_tex, sm::rect(512, 512), sm::Matrix2D(), false);
}

void ImageRenderer::Clear()
{
    m_tex.reset();
}

}