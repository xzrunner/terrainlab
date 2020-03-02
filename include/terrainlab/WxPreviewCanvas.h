#pragma once

#include "terrainlab/ImageRenderer.h"
#include "terrainlab/OverlayRenderer.h"

#include <ee0/typedef.h>
#include <ee3/WxStageCanvas.h>
#include <blueprint/typedef.h>
#include <blueprint/WxGraphPage.h>

#include <node0/typedef.h>
#include <terraingraph/typedef.h>
#include <terraingraph/DeviceVarType.h>

#include <array>

namespace terrainlab
{

class WxGraphPage;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
    WxPreviewCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);
    virtual ~WxPreviewCanvas();

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    void SetGraphPage(const bp::WxGraphPage<terraingraph::DeviceVarType>* graph_page);

    void InitEditOP(const ee0::EditOPPtr& default_op);

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

    bp::NodePtr GetSelectedNode() const;
    terraingraph::DevicePtr GetSelectedDevice() const;

private:
    enum OperatorID
    {
        OP_DEFAULT,
        OP_TEMP_BRUSH,
        OP_NOISE_BRUSH,
        OP_MAX_NUM
    };

private:
    const bp::WxGraphPage<terraingraph::DeviceVarType>* m_graph_page = nullptr;

    n0::SceneNodePtr m_selected = nullptr;

    std::shared_ptr<HeightfieldRenderer> m_hf_rd = nullptr;
    ImageRenderer   m_img_rd;
    OverlayRenderer m_overlay_rd;

    std::array<ee0::EditOPPtr, OP_MAX_NUM> m_ops;

}; // WxPreviewCanvas

}