#pragma once

#include "terrainlab/BrushDrawOP.h"

#include <SM_Matrix.h>

namespace pt3 { class WindowContext; }

namespace terrainlab
{

namespace node { class TemplateBrush; }

class TemplateBrushOP : public BrushDrawOP
{
public:
    TemplateBrushOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
        const ee0::SubjectMgrPtr& sub_mgr, const std::shared_ptr<rp::HeightfieldRenderer>& hf_rd);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseMove(int x, int y) override;
    virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

    void SetWindowContext(const std::shared_ptr<pt3::WindowContext>& wc) { m_wc = wc; }

private:
    sm::mat4 CalcBrushTrans(int x, int y) const;

private:
    sm::mat4 m_brush_trans;

    std::shared_ptr<pt3::WindowContext> m_wc = nullptr;

}; // TemplateBrushOP

}