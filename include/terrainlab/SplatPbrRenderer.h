#pragma once

#include <unirender/Texture.h>

#include "terrainlab/HeightfieldRenderer.h"

namespace terrainlab
{

class SplatPbrRenderer : public HeightfieldRenderer
{
public:
    SplatPbrRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;
    virtual void Setup(const std::shared_ptr<hf::HeightField>& hf) override;

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