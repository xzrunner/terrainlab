#pragma once

#include <unirender/Texture.h>

namespace wm { class Bitmap; class Mask; }

namespace wmv
{

class ImageRenderer
{
public:
    void Setup(const std::shared_ptr<wm::Bitmap>& bmp);
    void Setup(const std::shared_ptr<wm::Mask>& mask);

    void Draw() const;

    void Clear();

private:
    ur::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}