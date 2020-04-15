#pragma once

#include <memory>

namespace ur2 { class Device; class Context; }
namespace terraintiler { class GeoMipMapping; }

namespace terrainlab
{

class FullView2dRenderer
{
public:
    FullView2dRenderer();

    void Draw(const ur2::Device& dev, ur2::Context& ctx) const;

    void Clear();

private:
    std::shared_ptr<terraintiler::GeoMipMapping> m_mipmap = nullptr;

}; // FullView2dRenderer

}