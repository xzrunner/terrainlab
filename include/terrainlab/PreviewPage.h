#pragma once

#include <ee0/Observer.h>

#include <vector>
#include <array>

namespace ee0 { class WxStagePage; class RenderContext; }

namespace terrainlab
{

class PreviewPage : public ee0::Observer
{
public:
    PreviewPage(ee0::WxStagePage& stage_page, const ee0::RenderContext& rc);
    virtual ~PreviewPage();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
    void Init(const ee0::RenderContext& rc);

private:
    ee0::WxStagePage& m_stage_page;

    std::vector<uint32_t> m_messages;

}; // PreviewPage

}