#pragma once

#include <SM_Vector.h>

namespace terrainlab
{

struct RenderVertex
{
    RenderVertex() {}
	RenderVertex(size_t ix, size_t iz, size_t size_x, size_t size_z)
	{
		//static const float off = -1.0f;
		//static const float scale = 1 / 50.0f;

		//position.Set(off + ix * scale, 0, off + iz * scale);

        position.Set(
            ix / static_cast<float>(size_x),
            0,
            iz / static_cast<float>(size_z)
        );
		texcoords.Set(
            ix / static_cast<float>(size_x),
            iz / static_cast<float>(size_z)
        );
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

}