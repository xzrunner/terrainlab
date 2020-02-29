#pragma once

#include <terraingraph/typedef.h>
#include <terraingraph/DeviceVarType.h>

namespace terraingraph { class Device; }
namespace bp { class Node; class Pin; }

namespace terrainlab
{

class Evaluator;

class TerrainGraph
{
public:
    static void UpdatePropBackFromFront(const bp::Node& front,
        terraingraph::Device& back, const Evaluator& eval);

    static terraingraph::DevicePtr CreateBackFromFront(const bp::Node& front);

    static int TypeBackToFront(terraingraph::DeviceVarType type);

}; // SOP

}