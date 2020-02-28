#pragma once

#include <unirender/Texture.h>

#include "wmv/HeightfieldRenderer.h"

namespace wmv
{

class SplatPbrRenderer : public HeightfieldRenderer
{
public:
    SplatPbrRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;
    virtual void Setup(const std::shared_ptr<wm::HeightField>& hf) override;

private:
    void InitTextuers();
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;
    ur::TexturePtr m_normal_map = nullptr;
    ur::TexturePtr m_ao_map     = nullptr;

    ur::TexturePtr m_splat_map[4];

}; // SplatPbrRenderer

}