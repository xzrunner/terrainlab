#pragma once

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

#include <SM_Vector.h>

namespace clipmap { class Clipmap; }

namespace terrainlab
{

class ClipmapCamOP : public ee0::EditOP
{
public:
    ClipmapCamOP(const std::shared_ptr<pt0::Camera>& camera,
        const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnKeyDown(int key_code) override;
    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;
    virtual bool OnMouseWheelRotation(int x, int y, int direction) override;

    void SetVTex(const std::shared_ptr<clipmap::Clipmap>& vtex) {
        if (m_vtex != vtex) {
            m_vtex = vtex;
        }
    }

private:
    bool Translate(const sm::vec2& offset);
    bool Scale(float scale);

private:
    std::shared_ptr<clipmap::Clipmap> m_vtex = nullptr;

    ee0::SubjectMgrPtr m_sub_mgr;

    sm::ivec2 m_last_pos;

}; // ClipmapCamOP

}