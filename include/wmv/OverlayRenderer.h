#pragma once

#include <unirender/Texture.h>

#include "wmv/HeightfieldRenderer.h"

namespace wm { class Bitmap; }

namespace wmv
{

class OverlayRenderer : public HeightfieldRenderer
{
public:
    OverlayRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;

    void Setup(const std::shared_ptr<wm::HeightField>& hf,
        const std::shared_ptr<wm::Bitmap>& bmp);

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