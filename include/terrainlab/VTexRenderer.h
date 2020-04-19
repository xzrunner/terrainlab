#pragma once

#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

#include <memory>

namespace ur2 { class Device; class Context; }
namespace terraintiler { class VirtualTexture; }

namespace terrainlab
{

class VTexRenderer : private boost::noncopyable
{
public:
    VTexRenderer();

    void Draw(const ur2::Device& dev, ur2::Context& ctx,
        const sm::mat4& mt = sm::mat4()) const;

private:
    mutable std::shared_ptr<terraintiler::VirtualTexture> m_vtex = nullptr;

}; // VTexRenderer

}