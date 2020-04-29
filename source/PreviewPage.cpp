#include "terrainlab/PreviewPage.h"
#include "terrainlab/WxPreviewCanvas.h"

namespace terrainlab
{

PreviewPage::PreviewPage(const ur::Device& dev, ee0::WxStagePage& stage_page, const ee0::RenderContext& rc)
    : m_stage_page(stage_page)
{
    Init(dev, rc);
}

PreviewPage::~PreviewPage()
{
}

void PreviewPage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
}

void PreviewPage::Init(const ur::Device& dev, const ee0::RenderContext& rc)
{
    auto canvas = std::make_shared<WxPreviewCanvas>(dev, &m_stage_page, ECS_WORLD_VAR rc);
    m_stage_page.GetImpl().SetCanvas(canvas);

    canvas->InitEditOP();
}

}