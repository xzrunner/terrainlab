#pragma once

#include <unirender/Texture.h>

#include "wmv/HeightfieldRenderer.h"

namespace wmv
{

class SplatRenderer : public HeightfieldRenderer
{
public:
    SplatRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;

    void Setup(const std::shared_ptr<wm::HeightField>& hf);

    void Draw() const;

private:
    void InitTextuers();
    void InitShader();

private:
    ur::TexturePtr m_height_map = nullptr;

    ur::TexturePtr m_detail_map = nullptr;
    ur::TexturePtr m_splat_map[4];

}; // SplatRenderer

}