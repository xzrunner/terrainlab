#pragma once

#include <SM_Vector.h>
#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace terraintiler { class GeoMipMapping; }
namespace pt3 { class WindowContext; }
namespace ur { class Shader; }

namespace terrainlab
{

class FullView3dRenderer : private boost::noncopyable
{
public:
    FullView3dRenderer();

    void Setup(std::shared_ptr<pt3::WindowContext>& wc) const;

    void Update();
    void Draw(const sm::vec3& cam_pos, const sm::mat4& mt = sm::mat4(), 
        bool debug_draw = false) const;

private:
    void InitShader();

private:
    std::unique_ptr<ur::Shader> m_shader = nullptr;

    std::shared_ptr<terraintiler::GeoMipMapping> m_mipmap = nullptr;

}; // FullView3dRenderer

}