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

static const int SIZE = 128;

struct Vertex
{
    Vertex() {}
	Vertex(size_t ix, size_t iz)
	{
		static const float off = -1.0f;
		static const float scale = 1 / 50.0f;

		position.Set(off + ix * scale, 0, off + iz * scale);
		texcoords.Set(ix / static_cast<float>(SIZE), iz / static_cast<float>(SIZE));
	}

	sm::vec3 position;
	sm::vec2 texcoords;
};

}

class TerrRenderer : public rp::IRenderer, private rp::RendererImpl<render::Vertex>, private boost::noncopyable
{
public:
    TerrRenderer();

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

}; // TerrRenderer

}