#pragma once

#include <renderpipeline/HeightfieldRenderer.h>

namespace terrainlab
{

class SplatPbrRenderer : public rp::HeightfieldRenderer
{
public:
    SplatPbrRenderer(const ur::Device& dev);

    virtual void Flush(ur::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override;

private:
    void InitTextuers(const ur::Device& dev);
    void InitShader(const ur::Device& dev);

private:
    ur::TexturePtr m_height_map = nullptr;
    ur::TexturePtr m_normal_map = nullptr;
    ur::TexturePtr m_ao_map     = nullptr;

    ur::TexturePtr m_splat_map[4];

}; // SplatPbrRenderer

}