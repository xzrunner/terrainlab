#pragma once

#include <memory>

namespace ur { class Device; class Context; }
namespace terraintiler { class GeoMipMapping; }

namespace terrainlab
{

class FullView2dRenderer
{
public:
    FullView2dRenderer();

    void Draw(const ur::Device& dev, ur::Context& ctx) const;

    void Clear();

private:
    std::shared_ptr<terraintiler::GeoMipMapping> m_mipmap = nullptr;

}; // FullView2dRenderer

}