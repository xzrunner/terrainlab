#pragma once

#include <unirender2/typedef.h>
#include <terraingraph/typedef.h>

namespace terraingraph { class Mask; }
namespace ur2 { class Device; class Context; }

namespace terrainlab
{

class ImageRenderer
{
public:
    void Setup(const ur2::Device& dev, const terraingraph::BitmapPtr& bmp);
    void Setup(const ur2::Device& dev, const std::shared_ptr<terraingraph::Mask>& mask);

    void Draw(const ur2::Device& dev, ur2::Context& ctx) const;

    void Clear();

private:
    ur2::TexturePtr m_tex = nullptr;

}; // ImageRenderer

}