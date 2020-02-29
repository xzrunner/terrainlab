#pragma once

#include "terrainlab/RenderVertex.h"

#include <renderpipeline/IRenderer.h>
#include <renderpipeline/RendererImpl.h>
#include <SM_Matrix.h>

#include <boost/noncopyable.hpp>

namespace terraingraph { class HeightField; }

namespace terrainlab
{

class HeightfieldRenderer : public rp::IRenderer, public rp::RendererImpl<RenderVertex, uint32_t>, private boost::noncopyable
{
public:
    virtual void Clear();
    virtual void Setup(const std::shared_ptr<terraingraph::HeightField>& hf) = 0;

    void Draw(const sm::mat4& mt = sm::mat4()) const;

protected:
    void BuildVertBuf();
    void DrawVertBuf() const;

protected:
    std::shared_ptr<terraingraph::HeightField> m_hf = nullptr;

}; // HeightfieldRenderer

}