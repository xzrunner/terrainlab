#pragma once

#include <SM_Vector.h>

#include <memory>

namespace clipmap { class Clipmap; }
namespace ur2 { class Device; class Context; }

namespace terrainlab
{

class Clipmap2dRenderer
{
public:
    Clipmap2dRenderer() {}

    void Draw(const ur2::Device& dev, ur2::Context& ctx,
        float screen_width, float screen_height) const;

    auto GetVTex() const { return m_vtex; }

private:
    mutable std::shared_ptr<clipmap::Clipmap> m_vtex = nullptr;

}; // Clipmap2dRenderer

}