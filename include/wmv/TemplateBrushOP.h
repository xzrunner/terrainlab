#pragma once

#include <ee3/CameraDriveOP.h>

#include <node0/typedef.h>
#include <wm/device/TemplateBrush.h>

namespace wmv
{

class HeightfieldRenderer;
namespace node { class TemplateBrush; }

class TemplateBrushOP : public ee3::CameraDriveOP
{
public:
    TemplateBrushOP(const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseMove(int x, int y) override;
    virtual bool OnDraw() const override;

    void SetGroupSubMgr(const ee0::SubjectMgrPtr& group_sub_mgr) {
        m_group_sub_mgr = group_sub_mgr;
    }

    void SetBrush(const std::shared_ptr<node::TemplateBrush>& brush,
        const std::shared_ptr<wm::HeightField>& hf,
        const n0::SceneNodePtr& group_node);

    auto GetRenderer() const { return m_renderer; }

private:
    sm::mat4 CalcBrushTrans(int x, int y) const;

private:
    const pt3::Viewport& m_vp;
    ee0::SubjectMgrPtr m_group_sub_mgr;

    std::shared_ptr<node::TemplateBrush> m_brush = nullptr;
    n0::SceneNodePtr m_group_node = nullptr;

    std::shared_ptr<HeightfieldRenderer> m_renderer = nullptr;

    sm::mat4 m_brush_trans;

}; // TemplateBrushOP

}