#include "terrainlab/TerrainGraph.h"
#include "terrainlab/RegistNodes.h"
#include "terrainlab/PinType.h"
#include "terrainlab/Node.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>

#include <terraingraph/Device.h>

namespace terrainlab
{

void TerrainGraph::UpdatePropBackFromFront(const bp::Node& front, terraingraph::Device& back,
                                          const Evaluator& eval)
{
    auto f_type = front.get_type();
    auto b_type = back.get_type();
    if (f_type.is_derived_from<Node>() &&
        b_type.is_derived_from<terraingraph::Device>())
    {
        for (auto& dst_prop : b_type.get_properties())
        {
            auto src_prop = f_type.get_property(dst_prop.get_name());
            assert(src_prop.is_valid());
            dst_prop.set_value(back, src_prop.get_value(front));
        }
    }
}

terraingraph::DevicePtr TerrainGraph::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "terraingraph";
    auto find_lib = src_type.find("terrainlab::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("terrainlab::"));
    }

    terraingraph::DevicePtr dst = nullptr;

    if (!dst_type.empty())
    {
	    rttr::type t = rttr::type::get_by_name(dst_type);
        // fixme: specify node type
	    if (!t.is_valid())
        {
            assert(0);
	    }
        else
        {
            rttr::variant var = t.create();
            assert(var.is_valid());

            dst = var.get_value<std::shared_ptr<terraingraph::Device>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}


int TerrainGraph::TypeBackToFront(terraingraph::DeviceVarType type)
{
    int ret = -1;

    switch (type)
    {
    case terraingraph::DeviceVarType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case terraingraph::DeviceVarType::Heightfield:
        ret = PIN_HEIGHTFIELD;
        break;
    case terraingraph::DeviceVarType::Bitmap:
        ret = PIN_BITMAP;
        break;
    case terraingraph::DeviceVarType::Mask:
        ret = PIN_MASK;
        break;
    default:
        assert(0);
    }

    return ret;
}

}