#pragma once

#include <unirender2/typedef.h>
#include <renderpipeline/HeightfieldRenderer.h>
#include <SM_Matrix.h>

//#define BUILD_NORMAL_MAP
//#define BUILD_SHADOW_MAP

namespace terrainlab
{

class SplatRenderer : public rp::HeightfieldRenderer
{
public:
    SplatRenderer(const ur2::Device& dev);

    virtual void Flush(ur2::Context& ctx) override {}

    virtual void Clear() override;
    virtual void Setup(const ur2::Device& dev, ur2::Context& ctx,
        const std::shared_ptr<hf::HeightField>& hf) override;

private:
    void InitTextuers(const ur2::Device& dev);
    void InitShader(const ur2::Device& dev);

    void InitUniforms();

private:
    ur2::TexturePtr m_height_map = nullptr;
#ifdef BUILD_NORMAL_MAP
    ur2::TexturePtr m_normal_map = nullptr;
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
    ur2::TexturePtr m_shadow_map = nullptr;
#endif // BUILD_SHADOW_MAP

    ur2::TexturePtr m_noise_map;
    ur2::TexturePtr m_splat_map[4];

    sm::vec3 m_light_dir = sm::vec3(1, -1, 2);

    sm::vec4 m_layer_height_scale;
    sm::vec4 m_layer_height_bias;

    sm::vec4 m_layer_slope_scale;
    sm::vec4 m_layer_slope_bias;

    sm::mat4 m_layer_distort;

}; // SplatRenderer

}