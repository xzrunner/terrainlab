#pragma once

#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace ur { class Shader; }
namespace terraintiler { class Clipmap; }
namespace pt3 { class WindowContext; }

namespace terrainlab
{

class Clipmap3dRenderer : private boost::noncopyable
{
public:
    Clipmap3dRenderer();

    void Setup(std::shared_ptr<pt3::WindowContext>& wc) const;

    void Draw(const sm::mat4& mt = sm::mat4()) const;

private:
    void InitShader();

private:
    std::unique_ptr<ur::Shader> m_shader = nullptr;

    mutable std::shared_ptr<terraintiler::Clipmap> m_clipmap = nullptr;

}; // Clipmap3dRenderer

}