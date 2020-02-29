#include "wmv/NoiseBrushOP.h"
#include "wmv/RegistNodes.h"
#include "wmv/HeightfieldRenderer.h"

namespace wmv
{

NoiseBrushOP::NoiseBrushOP(const std::shared_ptr<pt0::Camera>& camera,
                                 const pt3::Viewport& vp,
                                 const ee0::SubjectMgrPtr& sub_mgr)
    : BrushDrawOP(camera, vp, sub_mgr)
{
}

bool NoiseBrushOP::OnMouseLeftDown(int x, int y)
{
    if (BrushDrawOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    m_left_pressed = true;

    AddToBrush(x, y);
    OnEditorChanged();

    return false;
}

bool NoiseBrushOP::OnMouseLeftUp(int x, int y)
{
    if (BrushDrawOP::OnMouseLeftUp(x, y)) {
        return true;
    }

    m_left_pressed = false;

    return false;
}

bool NoiseBrushOP::OnMouseDrag(int x, int y)
{
    if (BrushDrawOP::OnMouseDrag(x, y)) {
        return true;
    }

    if (m_left_pressed) {
        AddToBrush(x, y);
        OnEditorChanged();
    }

    return false;
}

void NoiseBrushOP::AddToBrush(int x, int y) const
{
    sm::vec3 pos;
    if (!CalcMousePos(pos, sm::ivec2(x, y))) {
        return;
    }

    assert(m_brush_node->get_type() == rttr::type::get<wmv::node::NoiseBrush>());
    auto brush = std::static_pointer_cast<wmv::node::NoiseBrush>(m_brush_node);
    if (brush->m_path.size() > 256) {
        brush->m_path.clear();
    }
    brush->m_path.push_back(sm::vec3(pos.x, pos.z, brush->m_radius));
}

}