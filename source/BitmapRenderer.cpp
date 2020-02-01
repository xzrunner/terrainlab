#include "terrview/BitmapRenderer.h"

#include <terr/Bitmap.h>
#include <model/TextureLoader.h>
#include <painting2/RenderSystem.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

namespace terrv
{

void BitmapRenderer::Setup(const std::shared_ptr<terr::Bitmap>& bmp)
{
    if (!bmp) {
        return;
    }

    auto& pixels = bmp->GetValues();
    if (pixels.empty()) {
        return;
    }

    auto w = bmp->Width();
    auto h = bmp->Height();
    assert(pixels.size() == w * h * 3);

    m_tex = model::TextureLoader::LoadFromMemory(
        pixels.data(), w, h, 3
    );
}

void BitmapRenderer::Draw() const
{
    if (!m_tex) {
        return;
    }

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    rc.SetZTest(ur::DEPTH_DISABLE);

    pt2::RenderSystem::DrawTexture(*m_tex, sm::rect(512, 512), sm::Matrix2D(), false);
}

}