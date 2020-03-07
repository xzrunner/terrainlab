#pragma once

#include "terrainlab/Node.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

class FullView2D : public Node
{
public:
    FullView2D()
        : Node("FullView2D")
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, PIN_HEIGHTFIELD, "FullView2D", *this
        ));

        Layout();
    }

private:

    RTTR_ENABLE(Node)

}; // FullView2D

}
}