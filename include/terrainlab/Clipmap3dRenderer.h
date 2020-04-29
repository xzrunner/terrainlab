#pragma once

#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace terraintiler { class Clipmap; }
namespace pt3 { class WindowContext; }
namespace clipmap { class Clipmap; }
namespace ur { class Device; class Context; class ShaderProgram; }

namespace terrainlab
{

class Clipmap3dRenderer : private boost::noncopyable
{
public:
    Clipmap3dRenderer(const ur::Device& dev);

    void Setup(std::shared_ptr<pt3::WindowContext>& wc) const;

    void Draw(const ur::Device& dev, ur::Context& ctx,
        const sm::mat4& mt = sm::mat4()) const;

    //std::shared_ptr<clipmap::Clipmap> GetVTex() const;
    auto GetVTex() const { return m_vtex; }

private:
    void InitShader(const ur::Device& dev);

    void DrawLayer(ur::Context& ctx, size_t start_level) const;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    mutable std::shared_ptr<terraintiler::Clipmap> m_vtex = nullptr;

}; // Clipmap3dRenderer

}