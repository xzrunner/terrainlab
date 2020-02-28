#include "wmv/TemplateBrushOP.h"
#include "wmv/GrayRenderer.h"
#include "wmv/SplatRenderer.h"
#include "wmv/RegistNodes.h"

#include <ee0/MsgHelper.h>
#include <blueprint/MessageID.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <painting0/Camera.h>
#include <painting3/PerspCam.h>
#include <painting3/Viewport.h>

namespace wmv
{

TemplateBrushOP::TemplateBrushOP(const std::shared_ptr<pt0::Camera>& camera,
                                 const pt3::Viewport& vp,
                                 const ee0::SubjectMgrPtr& sub_mgr)
    : ee3::CameraDriveOP(camera, vp, sub_mgr)
    , m_vp(vp)
{
    m_renderer = std::make_shared<SplatRenderer>();
}

bool TemplateBrushOP::OnMouseLeftDown(int x, int y)
{
    if (ee3::CameraDriveOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    if (!m_brush) {
        return false;
    }
    
    m_brush->m_matrixes.push_back(CalcBrushTrans(x, y));

    ee0::MsgHelper::SendObjMsg(*m_group_sub_mgr,
        m_group_node, bp::MSG_BP_NODE_PROP_CHANGED);

    return false;
}

bool TemplateBrushOP::OnMouseMove(int x, int y)
{
    if (ee3::CameraDriveOP::OnMouseMove(x, y)) {
        return true;
    }

    m_brush_trans = CalcBrushTrans(x, y);

    return false;
}

bool TemplateBrushOP::OnDraw() const
{
    if (ee3::CameraDriveOP::OnDraw()) {
        return true;
    }

    if (!m_brush) {
        return false;
    }

    m_renderer->Draw(m_brush_trans);

    return false;
}

void TemplateBrushOP::SetBrush(const std::shared_ptr<node::TemplateBrush>& brush,
                               const std::shared_ptr<wm::HeightField>& hf,
                               const n0::SceneNodePtr& group_node)
{
    if (m_brush == brush) {
        return;
    }

    m_brush = brush;

    m_renderer->Setup(hf);

    m_group_node = group_node;
}

sm::mat4 TemplateBrushOP::CalcBrushTrans(int x, int y) const
{
    auto cam_type = m_camera->TypeID();
    if (cam_type != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return sm::mat4();
    }

    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
    sm::vec3 ray_dir = m_vp.TransPos3ScreenToDir(
        sm::vec2(static_cast<float>(x), static_cast<float>(y)), *p_cam);
    sm::Ray ray(p_cam->GetPos(), ray_dir);

    sm::Plane plane(sm::vec3(0, 1, 0), sm::vec3(0, 0, 0));
    sm::vec3 cross;
    if (!sm::ray_plane_intersect(ray, plane, &cross)) {
        return sm::mat4();
    }

    auto s = m_brush->m_scale;
    auto s_mt = sm::mat4::Scaled(s, s, s);

    auto& r = m_brush->m_rotate;
    auto r_mt = sm::mat4::RotatedY(r);

    //auto& t = m_brush->m_translate;
    //auto t_mt = sm::mat4::Translated(cross.x + t.x, 0, cross.z + t.y);
    auto t_mt = sm::mat4::Translated(cross.x, 0, cross.z);

    auto o_mt = sm::mat4::Translated(-0.5f, 0, -0.5f);

    return t_mt * r_mt * s_mt * o_mt;
}

}