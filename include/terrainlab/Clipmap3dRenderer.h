#pragma once

#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace ur { class Shader; }
namespace terraintiler { class Clipmap; }
namespace pt3 { class WindowContext; }
namespace clipmap { class Clipmap; }

namespace terrainlab
{

class Clipmap3dRenderer : private boost::noncopyable
{
public:
    Clipmap3dRenderer();

    void Setup(std::shared_ptr<pt3::WindowContext>& wc) const;

    void Draw(const sm::mat4& mt = sm::mat4()) const;

    //std::shared_ptr<clipmap::Clipmap> GetVTex() const;
    auto GetVTex() const { return m_vtex; }

private:
    void InitShader();

    void DrawLayer(size_t start_level) const;

private:
    std::unique_ptr<ur::Shader> m_shader = nullptr;

    mutable std::shared_ptr<terraintiler::Clipmap> m_vtex = nullptr;

}; // Clipmap3dRenderer

}