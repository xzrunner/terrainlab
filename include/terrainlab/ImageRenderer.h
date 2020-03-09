#pragma once

#include <unirender/Texture.h>
#include <terraingraph/typedef.h>

namespace terraingraph { class Mask; }

namespace terrainlab
{

class ImageRenderer
{
public:
    void Setup(const terraingraph::BitmapPtr& bmp);
    void Setup(const std::shared_ptr<terraingraph::Mask>& mask);

    void Draw() const;

    void Clear();

private:
    ur::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}