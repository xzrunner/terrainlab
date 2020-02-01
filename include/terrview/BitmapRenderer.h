#pragma once

#include <unirender/Texture.h>

namespace terr { class Bitmap; }

namespace terrv
{

class BitmapRenderer
{
public:

    void Setup(const std::shared_ptr<terr::Bitmap>& bmp);

    void Draw() const;

private:
    ur::TexturePtr m_tex = nullptr;

}; // BitmapRenderer

}