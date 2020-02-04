#pragma once

#include <unirender/Texture.h>

#include "terrview/HeightfieldRenderer.h"

namespace terrv
{

class SplatRenderer : public HeightfieldRenderer
{
public:
    SplatRenderer();

    virtual void Flush() override {}

    virtual void Clear() override;

    void Setup(const std::shared_ptr<terr::HeightField>& hf);

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