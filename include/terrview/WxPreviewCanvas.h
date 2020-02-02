#pragma once

#include "terrview/SplatRenderer.h"
#include "terrview/BitmapRenderer.h"
#include "terrview/OverlayRenderer.h"

#include <ee0/typedef.h>
#include <ee3/WxStageCanvas.h>

#include <node0/typedef.h>

namespace terrv
{

class WxGraphPage;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);
    virtual ~WxPreviewCanvas();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetGraphPage(const WxGraphPage* graph_page);

protected:
    virtual void DrawBackground3D() const override;
    virtual void DrawForeground3D() const override;
    virtual void DrawForeground2D() const override;

private:
    void OnSelectionInsert(const ee0::VariantSet& variants);
    void OnSelectionClear(const ee0::VariantSet& variants);

    void DrawSelected(tess::Painter& pt, const sm::mat4& cam_mat,
        const pt0::RenderContext& rc) const;

    void SetupRenderer();

private:
    const WxGraphPage* m_graph_page = nullptr;

    n0::SceneNodePtr m_selected = nullptr;

    SplatRenderer m_hf_rd;
    BitmapRenderer      m_bmp_rd;
    OverlayRenderer     m_overlay_rd;

}; // WxPreviewCanvas

}