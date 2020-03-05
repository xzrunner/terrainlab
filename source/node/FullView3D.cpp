#include "terrainlab/node/FullView3D.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

FullView3D::FullView3D()
    : Node("FullView3D")
{
    m_all_input.push_back(std::make_shared<bp::Pin>(
        true, 0, PIN_HEIGHTFIELD, "FullView3D", *this
    ));

    Layout();
}

}
}