#pragma once

#include "wmv/RenderVertex.h"

#include <renderpipeline/IRenderer.h>
#include <renderpipeline/RendererImpl.h>

#include <boost/noncopyable.hpp>

namespace wm { class HeightField; }

namespace wmv
{

class HeightfieldRenderer : public rp::IRenderer, public rp::RendererImpl<RenderVertex, uint32_t>, private boost::noncopyable
{
protected:
    virtual void Clear();

    void BuildVertBuf();
    void DrawVertBuf() const;

protected:
    std::shared_ptr<wm::HeightField> m_hf = nullptr;

}; // HeightfieldRenderer

}