#pragma once

#include <unirender/Texture.h>

#include "wmv/HeightfieldRenderer.h"

namespace wmv
{

class GrayRenderer : public HeightfieldRenderer
{
public:
    GrayRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;
    virtual void Setup(const std::shared_ptr<wm::HeightField>& hf) override;
    virtual void Draw() const override;

private:
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;

}; // GrayRenderer

}