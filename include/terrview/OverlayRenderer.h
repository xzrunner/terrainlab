#pragma once

#include <unirender/Texture.h>

#include "terrview/HeightfieldRenderer.h"

namespace terr { class Bitmap; }

namespace terrv
{

class OverlayRenderer : public HeightfieldRenderer
{
public:
    OverlayRenderer();

    virtual void Flush() override {}

    void Setup(const std::shared_ptr<terr::HeightField>& hf,
        const std::shared_ptr<terr::Bitmap>& bmp);

    void Draw() const;

private:
    void InitTextuers();
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;

    ur::TexturePtr m_color_map  = nullptr;
    ur::TexturePtr m_detail_map = nullptr;

}; // OverlayRenderer

}