#pragma once

#include "terrainlab/Node.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

class VirtualTexture : public Node
{
public:
    VirtualTexture()
        : Node("VirtualTexture")
    {
        Layout();
    }

private:

    RTTR_ENABLE(Node)

}; // VirtualTexture

}
}