#pragma once

#include <renderpipeline/HeightfieldRenderer.h>
#include <terraingraph/typedef.h>

namespace terrainlab
{

class OverlayRenderer : public rp::HeightfieldRenderer
{
public:
    OverlayRenderer(const ur2::Device& dev);

    virtual void Flush(ur2::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur2::Device& dev, ur2::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override {}

    void Setup(const ur2::Device& dev, ur2::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf,
        const terraingraph::BitmapPtr& bmp);

private:
    void InitShader(const ur2::Device& dev);

private:
    ur2::TexturePtr m_height_map = nullptr;
    ur2::TexturePtr m_color_map  = nullptr;

}; // OverlayRenderer

}