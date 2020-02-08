#include "wmv/PinCallback.h"
#include "wmv/PinType.h"

#include <painting0/Color.h>
#include <blueprint/Pin.h>

#include <string>

namespace
{

const pt0::Color COL_DEFAULT   = pt0::Color(255, 255, 255);

const pt0::Color COL_HEIGHTFIELD = pt0::Color(132, 228, 231);
const pt0::Color COL_BITMAP      = pt0::Color(154, 239, 146);
const pt0::Color COL_MASK        = pt0::Color(246, 255, 154);

std::string get_desc_func(const std::string& name, int type)
{
    std::string ret = name;
    switch (type)
    {
    case wmv::PIN_HEIGHTFIELD:
        ret += "(H)";
        break;
    case wmv::PIN_BITMAP:
        ret += "(B)";
        break;
    case wmv::PIN_MASK:
        ret += "(M)";
        break;
    }
    return ret;
}

const pt0::Color& get_color_func(int type)
{
    switch (type)
    {
    case wmv::PIN_HEIGHTFIELD:
        return COL_HEIGHTFIELD;
    case wmv::PIN_BITMAP:
        return COL_BITMAP;
    case wmv::PIN_MASK:
        return COL_MASK;
    default:
        return COL_DEFAULT;
    }
}

bool can_type_cast_func(int type_from, int type_to)
{
    if (type_from == type_to) {
        return true;
    }
    if (type_from == bp::PIN_ANY_VAR ||
        type_to == bp::PIN_ANY_VAR) {
        return true;
    }

    return false;
}

}

namespace wmv
{

void InitPinCallback()
{
    bp::Pin::SetExtendFuncs({
        get_desc_func,
        get_color_func,
        can_type_cast_func
    });
}

}