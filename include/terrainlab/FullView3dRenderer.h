#pragma once

#include <SM_Vector.h>
#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace terraintiler { class GeoMipMapping; }
namespace pt3 { class WindowContext; }
namespace ur { class Device; class Context; class ShaderProgram; }

namespace terrainlab
{

class FullView3dRenderer : private boost::noncopyable
{
public:
    FullView3dRenderer(const ur::Device& dev);

    void Setup(std::shared_ptr<pt3::WindowContext>& wc) const;

    void Update(const ur::Device& dev);
    void Draw(ur::Context& ctx, const sm::vec3& cam_pos,
        const sm::mat4& mt = sm::mat4(), bool debug_draw = false) const;

private:
    void InitShader(const ur::Device& dev);

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    std::shared_ptr<terraintiler::GeoMipMapping> m_mipmap = nullptr;

}; // FullView3dRenderer

}