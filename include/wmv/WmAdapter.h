#pragma once

#include <wm/typedef.h>
#include <wm/DeviceVarType.h>

namespace wm { class Device; }
namespace bp { class Node; class Pin; }

namespace wmv
{

class Evaluator;

class WmAdapter
{
public:
    static void UpdatePropBackFromFront(const bp::Node& front,
        wm::Device& back, const Evaluator& eval);

    static wm::DevicePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(wm::DeviceVarType type);

}; // SOP

}