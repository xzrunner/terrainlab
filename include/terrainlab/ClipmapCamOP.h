#pragma once

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

#include <SM_Vector.h>

namespace clipmap { class Clipmap; }
namespace terraintiler { class Clipmap; }

namespace terrainlab
{

namespace node { class Clipmap; }

class ClipmapCamOP : public ee0::EditOP
{
public:
    ClipmapCamOP(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<pt0::Camera>& camera,
        const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnKeyDown(int key_code) override;
    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;
    virtual bool OnMouseWheelRotation(int x, int y, int direction) override;

    //void SetVTex(const std::shared_ptr<clipmap::Clipmap>& vtex) {
    //    if (m_vtex != vtex) {
    //        m_vtex = vtex;
    //    }
    //}
    void SetVTex(const std::shared_ptr<terraintiler::Clipmap>& vtex);

    void SetClipmapNode(const std::shared_ptr<node::Clipmap>& clipmap_node) {
        m_clipmap_node = clipmap_node;
    }

private:
    bool Translate(const sm::vec2& offset);
    bool Scale(float scale);

private:
    const ur::Device& m_dev;
    ur::Context& m_ctx;

//    std::shared_ptr<clipmap::Clipmap> m_vtex = nullptr;
    std::shared_ptr<terraintiler::Clipmap> m_vtex = nullptr;

    ee0::SubjectMgrPtr m_sub_mgr;

    sm::ivec2 m_last_pos;

    std::shared_ptr<node::Clipmap> m_clipmap_node = nullptr;

}; // ClipmapCamOP

}