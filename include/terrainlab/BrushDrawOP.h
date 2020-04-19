#pragma once

#include <ee3/CameraDriveOP.h>

#include <node0/typedef.h>

namespace hf { class HeightField; }
namespace rp { class HeightfieldRenderer; }
namespace ur2 { class Device; class Context; }

namespace terrainlab
{

class Node;

class BrushDrawOP : public ee3::CameraDriveOP
{
public:
    BrushDrawOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
        const ee0::SubjectMgrPtr& sub_mgr, const std::shared_ptr<rp::HeightfieldRenderer>& hf_rd);

    auto GetRenderer() const { return m_renderer; }

    void Setup(const ur2::Device& dev, ur2::Context& ctx, const std::shared_ptr<Node>& brush_node,
        const std::shared_ptr<hf::HeightField>& hf, const n0::SceneNodePtr& editor_node);

    void SetEditorSubMgr(const ee0::SubjectMgrPtr& editor_sub_mgr) {
        m_editor_sub_mgr = editor_sub_mgr;
    }

protected:
    bool CalcMousePos(sm::vec3& proj_pos, const sm::ivec2& screen_pos) const;

    void OnEditorChanged();

protected:
    std::shared_ptr<rp::HeightfieldRenderer> m_renderer = nullptr;

    std::shared_ptr<Node> m_brush_node = nullptr;

private:
    const pt3::Viewport& m_vp;
    ee0::SubjectMgrPtr m_sub_mgr = nullptr;

    ee0::SubjectMgrPtr m_editor_sub_mgr;

    n0::SceneNodePtr m_editor_node = nullptr;

}; // BrushDrawOP

}