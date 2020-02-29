#pragma once

#include "terrainlab/BrushDrawOP.h"

#include <SM_Matrix.h>

namespace terrainlab
{

namespace node { class TemplateBrush; }

class TemplateBrushOP : public BrushDrawOP
{
public:
    TemplateBrushOP(const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseMove(int x, int y) override;
    virtual bool OnDraw() const override;

private:
    sm::mat4 CalcBrushTrans(int x, int y) const;

private:
    sm::mat4 m_brush_trans;

}; // TemplateBrushOP

}