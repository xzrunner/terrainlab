#pragma once

#include "terrainlab/Node.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

class FullView3D : public Node
{
public:
    FullView3D()
        : Node("FullView3D")
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, PIN_HEIGHTFIELD, "FullView3D", *this
        ));

        Layout();
    }

private:

    RTTR_ENABLE(Node)

}; // FullView3D

}
}