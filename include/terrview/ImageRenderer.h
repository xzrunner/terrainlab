#pragma once

#include <unirender/Texture.h>

namespace terr { class Bitmap; class Mask; }

namespace terrv
{

class ImageRenderer
{
public:
    void Setup(const std::shared_ptr<terr::Bitmap>& bmp);
    void Setup(const std::shared_ptr<terr::Mask>& mask);

    void Draw() const;

    void Clear();

private:
    ur::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}