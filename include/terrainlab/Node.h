#pragma once

#include <blueprint/Node.h>

namespace terrainlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

    virtual void Draw(const ur::Device& dev, ur::Context& ctx,
        const n2::RenderParams& rp) const override;

public:
    static constexpr char* const STR_PROP_DISPLAY = "Display";

protected:
    void InitPins(const std::string& name);

    RTTR_ENABLE(bp::Node)

}; // Node

}