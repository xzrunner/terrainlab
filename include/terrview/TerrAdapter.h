#pragma once

#include <terr/typedef.h>
#include <terr/DeviceVarType.h>

namespace terr { class Device; }
namespace bp { class Node; class Pin; }

namespace terrv
{

class Evaluator;

class TerrAdapter
{
public:
    static void UpdatePropBackFromFront(const bp::Node& front,
        terr::Device& back, const Evaluator& eval);

    static terr::DevicePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(terr::DeviceVarType type);

}; // SOP

}