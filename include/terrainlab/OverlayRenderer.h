#pragma once

#include <renderpipeline/HeightfieldRenderer.h>
#include <terraingraph/typedef.h>

namespace terrainlab
{

class OverlayRenderer : public rp::HeightfieldRenderer
{
public:
    OverlayRenderer(const ur::Device& dev);

    virtual void Flush(ur::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override {}

    void Setup(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf,
        const terraingraph::BitmapPtr& bmp);

private:
    void InitShader(const ur::Device& dev);

private:
    ur::TexturePtr m_height_map = nullptr;
    ur::TexturePtr m_color_map  = nullptr;

}; // OverlayRenderer

}