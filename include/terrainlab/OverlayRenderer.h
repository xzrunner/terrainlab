#pragma once

#include <unirender/Texture.h>
#include <renderpipeline/HeightfieldRenderer.h>

namespace terraingraph { class Bitmap; }

namespace terrainlab
{

class OverlayRenderer : public rp::HeightfieldRenderer
{
public:
    OverlayRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;
    virtual void Setup(const std::shared_ptr<hf::HeightField>& hf) override {}

    void Setup(const std::shared_ptr<hf::HeightField>& hf,
        const std::shared_ptr<terraingraph::Bitmap>& bmp);

private:
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;
    ur::TexturePtr m_color_map  = nullptr;

}; // OverlayRenderer

}