#include "terrainlab/ClipmapCamOP.h"
#include "terrainlab/node/Clipmap.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee2/CamZoomState.h>
#include <ee2/CamTranslateState.h>

#include <painting0/Camera.h>
#include <painting2/OrthoCamera.h>
#include <painting3/PerspCam.h>
#include <clipmap/Clipmap.h>
#include <terraintiler/Clipmap.h>

#include <wx/defs.h>

namespace
{

const float CAM3D_SCALE = 0.01f;

}

namespace terrainlab
{

ClipmapCamOP::ClipmapCamOP(const ur::Device& dev, ur::Context& ctx,
                           const std::shared_ptr<pt0::Camera>& camera,
                           const ee0::SubjectMgrPtr& sub_mgr)
    : ee0::EditOP(camera)
    , m_dev(dev)
    , m_ctx(ctx)
    , m_sub_mgr(sub_mgr)
{
    m_last_pos.MakeInvalid();
}

bool ClipmapCamOP::OnKeyDown(int key_code)
{
    if (ee0::EditOP::OnKeyDown(key_code)) {
        return true;
    }

    if (!m_vtex) {
        return false;
    }

    bool dirty = false;
	switch (key_code)
	{
	case WXK_ESCAPE:
    {
        auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
        if (m_clipmap_node) {
            p_cam->Reset(m_clipmap_node->cam_pos, m_clipmap_node->cam_target, m_clipmap_node->cam_up);
        } else {
            p_cam->Reset(sm::vec3(0, 0, -2), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0));
        }

        m_vtex->Update(m_dev, m_ctx, 1.0f, sm::vec2(0, 0));
        dirty = true;
    }
		break;
    case WXK_LEFT:
        if (Translate(sm::vec2(-10, 0))) {
            dirty = true;
        }
        break;
    case WXK_RIGHT:
        if (Translate(sm::vec2(10, 0))) {
            dirty = true;
        }
        break;
    case WXK_DOWN:
        if (Translate(sm::vec2(0, -10))) {
            dirty = true;
        }
        break;
    case WXK_UP:
        if (Translate(sm::vec2(0, 10))) {
            dirty = true;
        }
        break;
    case WXK_PAGEDOWN:
        if (Scale(1.1f)) {
            dirty = true;
        }
        break;
    case WXK_PAGEUP:
        if (Scale(0.9f)) {
            dirty = true;
        }
        break;
	}

    if (dirty) {
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    }

	return false;
}

bool ClipmapCamOP::OnMouseLeftDown(int x, int y)
{
    if (ee0::EditOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    m_last_pos.Set(x, y);

    return false;
}

bool ClipmapCamOP::OnMouseDrag(int x, int y)
{
    if (ee0::EditOP::OnMouseDrag(x, y)) {
        return true;
    }

    if (!m_last_pos.IsValid()) {
        return false;
    }

    Translate(sm::vec2(m_last_pos.x - x, y - m_last_pos.y));
    m_last_pos.Set(x, y);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

    return false;
}

bool ClipmapCamOP::OnMouseWheelRotation(int x, int y, int direction)
{
    if (ee0::EditOP::OnMouseWheelRotation(x, y, direction)) {
        return true;
    }

    const float scale = direction > 0 ? 1 / 1.1f : 1.1f;
    Scale(scale);

    m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

    return false;
}

void ClipmapCamOP::SetVTex(const std::shared_ptr<terraintiler::Clipmap>& vtex)
{
    if (m_vtex != vtex) {
        m_vtex = vtex;
    }
    if (m_vtex)
    {
        m_vtex->GetVTex()->Init(m_dev);

        m_camera->Reset();
        m_vtex->Update(m_dev, m_ctx, 1.0f, sm::vec2(0, 0));
    }
}

bool ClipmapCamOP::Translate(const sm::vec2& offset)
{
    if (!m_vtex) {
        return false;
    }

    const auto type = m_camera->TypeID();
    if (type == pt0::GetCamTypeID<pt2::OrthoCamera>())
    {
        auto o_cam = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera);

        float s = o_cam->GetScale();
        sm::vec2 p = o_cam->GetPosition() + offset * s;
        m_vtex->Update(m_dev, m_ctx, s, p);
        m_vtex->GetVTex()->GetRegion(s, p);
        o_cam->Set(p, s);
    }
    else if (type == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        float scale;
        sm::vec2 pos;
        m_vtex->GetVTex()->GetRegion(scale, pos);
        m_vtex->Update(m_dev, m_ctx, scale, pos + offset);

        //auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
        //p_cam->Translate(offset.x * CAM3D_SCALE, offset.y * CAM3D_SCALE);
    }

    return true;
}

bool ClipmapCamOP::Scale(float scale)
{
    if (!m_vtex) {
        return false;
    }

    const auto type = m_camera->TypeID();
    if (type == pt0::GetCamTypeID<pt2::OrthoCamera>())
    {
        auto o_cam = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera);

        float s = o_cam->GetScale() * scale;
        sm::vec2 p = o_cam->GetPosition();
        m_vtex->Update(m_dev, m_ctx, s, p);

        m_vtex->GetVTex()->GetRegion(s, p);
        o_cam->Set(p, s);
    }
    else if (type == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        float s;
        sm::vec2 pos;
        m_vtex->GetVTex()->GetRegion(s, pos);
        m_vtex->Update(m_dev, m_ctx, s * scale, pos);

        //auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
        //auto dist = p_cam->GetDistance();
        //p_cam->MoveToward(dist - dist * scale);
    }

    return true;
}

}