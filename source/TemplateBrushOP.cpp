#include "wmv/TemplateBrushOP.h"
#include "wmv/RegistNodes.h"
#include "wmv/HeightfieldRenderer.h"

namespace wmv
{

TemplateBrushOP::TemplateBrushOP(const std::shared_ptr<pt0::Camera>& camera,
                                 const pt3::Viewport& vp,
                                 const ee0::SubjectMgrPtr& sub_mgr)
    : BrushDrawOP(camera, vp, sub_mgr)
{
}

bool TemplateBrushOP::OnMouseLeftDown(int x, int y)
{
    if (BrushDrawOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    if (!m_brush_node) {
        return false;
    }

    assert(m_brush_node->get_type() == rttr::type::get<wmv::node::TemplateBrush>());
    std::static_pointer_cast<wmv::node::TemplateBrush>(m_brush_node)
        ->m_matrixes.push_back(CalcBrushTrans(x, y));

    OnEditorChanged();

    return false;
}

bool TemplateBrushOP::OnMouseMove(int x, int y)
{
    if (BrushDrawOP::OnMouseMove(x, y)) {
        return true;
    }

    m_brush_trans = CalcBrushTrans(x, y);

    return false;
}

bool TemplateBrushOP::OnDraw() const
{
    if (BrushDrawOP::OnDraw()) {
        return true;
    }

    if (!m_brush_node) {
        return false;
    }

    m_renderer->Draw(m_brush_trans);

    return false;
}

sm::mat4 TemplateBrushOP::CalcBrushTrans(int x, int y) const
{
    sm::vec3 pos;
    if (!CalcMousePos(pos, sm::ivec2(x, y))) {
        return sm::mat4();
    }

    assert(m_brush_node->get_type() == rttr::type::get<wmv::node::TemplateBrush>());
    auto brush = std::static_pointer_cast<wmv::node::TemplateBrush>(m_brush_node);

    auto s = brush->m_scale;
    auto s_mt = sm::mat4::Scaled(s, s, s);

    auto& r = brush->m_rotate;
    auto r_mt = sm::mat4::RotatedY(r);

    //auto& t = brush->m_translate;
    //auto t_mt = sm::mat4::Translated(cross.x + t.x, 0, cross.z + t.y);
    auto t_mt = sm::mat4::Translated(pos.x, 0, pos.z);

    auto o_mt = sm::mat4::Translated(-0.5f, 0, -0.5f);

    return t_mt * r_mt * s_mt * o_mt;
}

}