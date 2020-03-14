#pragma once

#include <SM_Vector.h>

#include <memory>

namespace clipmap { class Clipmap; }

namespace terrainlab
{

class Clipmap2dRenderer
{
public:
    Clipmap2dRenderer() {}

    void Draw(float scale, const sm::vec2& offset,
        float screen_width, float screen_height) const;

private:
    mutable std::shared_ptr<clipmap::Clipmap> m_tex = nullptr;

}; // Clipmap2dRenderer

}