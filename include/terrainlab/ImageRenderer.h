#pragma once

#include <unirender/Texture.h>

namespace terraingraph { class Bitmap; class Mask; }

namespace terrainlab
{

class ImageRenderer
{
public:
    void Setup(const std::shared_ptr<terraingraph::Bitmap>& bmp);
    void Setup(const std::shared_ptr<terraingraph::Mask>& mask);

    void Draw() const;

    void Clear();

private:
    ur::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}