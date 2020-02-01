#include "terrview/TerrAdapter.h"
#include "terrview/RegistNodes.h"
#include "terrview/PinType.h"
#include "terrview/Node.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>

#include <terr/Device.h>

namespace terrv
{

void TerrAdapter::UpdatePropBackFromFront(const bp::Node& front, terr::Device& back,
                                          const Evaluator& eval)
{
    auto f_type = front.get_type();
    auto b_type = back.get_type();
    if (f_type.is_derived_from<Node>() &&
        b_type.is_derived_from<terr::Device>())
    {
        for (auto& dst_prop : b_type.get_properties())
        {
            auto src_prop = f_type.get_property(dst_prop.get_name());
            assert(src_prop.is_valid());
            dst_prop.set_value(back, src_prop.get_value(front));
        }
    }
}

terr::DevicePtr TerrAdapter::CreateBackFromFront(const bp::Node& node)
{
    auto type = node.get_type();
    auto src_type = type.get_name().to_string();
    std::string dst_type;
    std::string lib_str = "terr";
    auto find_lib = src_type.find("terrv::");
    if (find_lib != std::string::npos) {
        dst_type = lib_str + "::" + src_type.substr(find_lib + strlen("terrv::"));
    }

    terr::DevicePtr dst = nullptr;

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

            dst = var.get_value<std::shared_ptr<terr::Device>>();
            assert(dst);
        }
    }

    if (node.get_type().is_derived_from<Node>()) {
        dst->SetName(static_cast<const Node&>(node).GetName());
    }

    return dst;
}


int TerrAdapter::TypeBackToFront(terr::DeviceVarType type)
{
    int ret = -1;

    switch (type)
    {
    case terr::DeviceVarType::Any:
        ret = bp::PIN_ANY_VAR;
        break;
    case terr::DeviceVarType::Heightfield:
        ret = PIN_HEIGHTFIELD;
        break;
    case terr::DeviceVarType::Bitmap:
        ret = PIN_BITMAP;
        break;
    default:
        assert(0);
    }

    return ret;
}

}