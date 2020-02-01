#pragma once

#include <SM_Vector.h>
#include <unirender/Texture.h>
#include <renderpipeline/IRenderer.h>
#include <renderpipeline/RendererImpl.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace terr { class HeightField; }

namespace terrv
{

namespace render
{

struct Vertex
{
    Vertex() {}
	Vertex(size_t ix, size_t iz, size_t size_x, size_t size_z)
	{
		static const float off = -1.0f;
		static const float scale = 1 / 50.0f;

		position.Set(off + ix * scale, 0, off + iz * scale);
		texcoords.Set(
            ix / static_cast<float>(size_x),
            iz / static_cast<float>(size_z)
        );
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

}

class HeightfieldRenderer : public rp::IRenderer, private rp::RendererImpl<render::Vertex, uint32_t>, private boost::noncopyable
{
public:
    HeightfieldRenderer();

    virtual void Flush() override {}

    void Setup(const std::shared_ptr<terr::HeightField>& hf);

    void Draw() const;

private:
    void InitTextuers();
    void InitShader();

private:
    std::shared_ptr<terr::HeightField> m_hf = nullptr;

    ur::TexturePtr m_height_map = nullptr;

    ur::TexturePtr m_detail_map = nullptr;
    ur::TexturePtr m_splat_map[4];

}; // HeightfieldRenderer

}