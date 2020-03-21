#include "terrainlab/ClipmapCamOP.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee2/CamZoomState.h>
#include <ee2/CamTranslateState.h>

#include <painting0/Camera.h>
#include <painting2/OrthoCamera.h>
#include <clipmap/Clipmap.h>

#include <wx/defs.h>

namespace terrainlab
{

ClipmapCamOP::ClipmapCamOP(const std::shared_ptr<pt0::Camera>& camera,
                           const ee0::SubjectMgrPtr& sub_mgr)
    : ee0::EditOP(camera)
    , m_sub_mgr(sub_mgr)
{
    m_last_pos.MakeInvalid();
}

bool ClipmapCamOP::OnKeyDown(int key_code)
{
    if (ee0::EditOP::OnKeyDown(key_code)) {
        return true;
    }

    bool dirty = false;
	switch (key_code)
	{
	case WXK_ESCAPE:
		m_camera->Reset();
        m_vtex->Update(1.0f, sm::vec2(0, 0));
        dirty = true;
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

bool ClipmapCamOP::Translate(const sm::vec2& offset)
{
    if (!m_vtex) {
        return false;
    }

    if (m_camera->TypeID() != pt0::GetCamTypeID<pt2::OrthoCamera>()) {
        return false;
    }

    auto o_cam = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera);

    float s = o_cam->GetScale();
    sm::vec2 p = o_cam->GetPosition() + offset * s;
    m_vtex->Update(s, p);
    m_vtex->GetRegion(s, p);
    o_cam->Set(p, s);

    return true;
}

bool ClipmapCamOP::Scale(float scale)
{
    if (!m_vtex) {
        return false;
    }

    if (m_camera->TypeID() != pt0::GetCamTypeID<pt2::OrthoCamera>()) {
        return false;
    }

    auto o_cam = std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera);

    float s = o_cam->GetScale() * scale;
    sm::vec2 p = o_cam->GetPosition();
    m_vtex->Update(s, p);
    m_vtex->GetRegion(s, p);
    o_cam->Set(p, s);

    return true;
}

}