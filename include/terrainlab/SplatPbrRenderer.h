#pragma once

#include <renderpipeline/HeightfieldRenderer.h>

namespace terrainlab
{

class SplatPbrRenderer : public rp::HeightfieldRenderer
{
public:
    SplatPbrRenderer(const ur2::Device& dev);

    virtual void Flush(ur2::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur2::Device& dev, ur2::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override;

private:
    void InitTextuers(const ur2::Device& dev);
    void InitShader(const ur2::Device& dev);

private:
    ur2::TexturePtr m_height_map = nullptr;
    ur2::TexturePtr m_normal_map = nullptr;
    ur2::TexturePtr m_ao_map     = nullptr;

    ur2::TexturePtr m_splat_map[4];

}; // SplatPbrRenderer

}