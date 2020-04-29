#pragma once

#include <unirender/typedef.h>
#include <terraingraph/typedef.h>

namespace terraingraph { class Mask; }
namespace ur { class Device; class Context; }

namespace terrainlab
{

class ImageRenderer
{
public:
    void Setup(const ur::Device& dev, const terraingraph::BitmapPtr& bmp);
    void Setup(const ur::Device& dev, const std::shared_ptr<terraingraph::Mask>& mask);

    void Draw(const ur::Device& dev, ur::Context& ctx) const;

    void Clear();

private:
    ur::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}