#pragma once

#include <unirender/typedef.h>
#include <renderpipeline/HeightfieldRenderer.h>
#include <SM_Matrix.h>

//#define BUILD_NORMAL_MAP
//#define BUILD_SHADOW_MAP

namespace terrainlab
{

class SplatRenderer : public rp::HeightfieldRenderer
{
public:
    SplatRenderer(const ur::Device& dev);

    virtual void Flush(ur::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur::Device& dev, ur::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override;

protected:
    virtual void BeforeDraw(ur::Context& ctx) const override;

private:
    void InitTextuers(const ur::Device& dev);
    void InitShader(const ur::Device& dev);

    void InitUniforms();

private:
    ur::TexturePtr m_height_map = nullptr;
#ifdef BUILD_NORMAL_MAP
    ur::TexturePtr m_normal_map = nullptr;
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
    ur::TexturePtr m_shadow_map = nullptr;
#endif // BUILD_SHADOW_MAP

    ur::TexturePtr m_noise_map;
    ur::TexturePtr m_splat_map[4];

    sm::vec3 m_light_dir = sm::vec3(1, -1, 2);

    sm::vec4 m_layer_height_scale;
    sm::vec4 m_layer_height_bias;

    sm::vec4 m_layer_slope_scale;
    sm::vec4 m_layer_slope_bias;

    sm::mat4 m_layer_distort;

}; // SplatRenderer

}