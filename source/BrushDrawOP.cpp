#include "terrainlab/BrushDrawOP.h"
#include "terrainlab/SplatRenderer.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/MessageID.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <painting0/Camera.h>
#include <painting3/PerspCam.h>
#include <painting3/Viewport.h>

namespace terrainlab
{

BrushDrawOP::BrushDrawOP(const std::shared_ptr<pt0::Camera>& camera,
                         const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr,
                         const std::shared_ptr<rp::HeightfieldRenderer>& hf_rd)
    : ee3::CameraDriveOP(camera, vp, sub_mgr)
    , m_vp(vp)
    , m_sub_mgr(sub_mgr)
    , m_renderer(hf_rd)
{
}

void BrushDrawOP::Setup(const ur2::Device& dev, ur2::Context& ctx,
                        const std::shared_ptr<Node>& brush_node,
                        const std::shared_ptr<hf::HeightField>& hf,
                        const n0::SceneNodePtr& editor_node)
{
    if (m_brush_node == brush_node) {
        return;
    }

    m_brush_node = brush_node;

    m_renderer->Setup(dev, ctx, hf);

    m_editor_node = editor_node;
}

bool BrushDrawOP::CalcMousePos(sm::vec3& proj_pos, const sm::ivec2& screen_pos) const
{
    auto cam_type = m_camera->TypeID();
    if (cam_type != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }

    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
    sm::vec3 ray_dir = m_vp.TransPos3ScreenToDir(
        sm::vec2(static_cast<float>(screen_pos.x), static_cast<float>(screen_pos.y)), *p_cam);
    sm::Ray ray(p_cam->GetPos(), ray_dir);

    sm::Plane plane(sm::vec3(0, 1, 0), sm::vec3(0, 0, 0));
    sm::vec3 cross;
    if (!sm::ray_plane_intersect(ray, plane, &cross)) {
        return false;
    }

    proj_pos = cross;

    return true;
}

void BrushDrawOP::OnEditorChanged()
{
    ee0::MsgHelper::SendObjMsg(*m_editor_sub_mgr,
        m_editor_node, bp::MSG_BP_NODE_PROP_CHANGED);
    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}