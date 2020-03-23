#pragma once

#include "terrainlab/Node.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

class Clipmap : public Node
{
public:
    Clipmap()
        : Node("Clipmap")
    {
        m_all_input.push_back(std::make_shared<bp::Pin>(
            true, 0, PIN_HEIGHTFIELD, "Clipmap", *this
        ));

        Layout();
    }

public:
    sm::vec3 cam_pos;
    sm::vec3 cam_target;
    sm::vec3 cam_up;

    RTTR_ENABLE(Node)

}; // Clipmap

}
}