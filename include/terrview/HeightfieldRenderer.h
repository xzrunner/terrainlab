#pragma once

#include "terrview/RenderVertex.h"

#include <renderpipeline/IRenderer.h>
#include <renderpipeline/RendererImpl.h>

#include <boost/noncopyable.hpp>

namespace terr { class HeightField; }

namespace terrv
{

class HeightfieldRenderer : public rp::IRenderer, public rp::RendererImpl<RenderVertex, uint32_t>, private boost::noncopyable
{
protected:
    void BuildVertBuf();
    void DrawVertBuf() const;

protected:
    std::shared_ptr<terr::HeightField> m_hf = nullptr;

}; // HeightfieldRenderer

}