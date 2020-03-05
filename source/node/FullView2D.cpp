#include "terrainlab/node/FullView2D.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>

namespace terrainlab
{
namespace node
{

FullView2D::FullView2D()
    : Node("FullView2D")
{
    m_all_input.push_back(std::make_shared<bp::Pin>(
        true, 0, PIN_HEIGHTFIELD, "FullView2D", *this
    ));

    Layout();
}

}
}