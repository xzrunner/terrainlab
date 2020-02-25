#pragma once

#include <unirender/Texture.h>

#include "wmv/HeightfieldRenderer.h"

#define BUILD_NORMAL_MAP
//#define BUILD_SHADOW_MAP

namespace wmv
{

class SplatRenderer : public HeightfieldRenderer
{
public:
    SplatRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;
    virtual void Setup(const std::shared_ptr<wm::HeightField>& hf) override;
    virtual void Draw() const override;

private:
    void InitTextuers();
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;
#ifdef BUILD_NORMAL_MAP
    ur::TexturePtr m_normal_map = nullptr;
#endif // BUILD_NORMAL_MAP
#ifdef BUILD_SHADOW_MAP
    ur::TexturePtr m_shadow_map = nullptr;
#endif // BUILD_SHADOW_MAP

    ur::TexturePtr m_splat_map[4];

    sm::vec3 m_light_dir = sm::vec3(1, -1, 2);

}; // SplatRenderer

}