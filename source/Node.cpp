#include "terrainlab/Node.h"
#include "terrainlab/TerrainGraph.h"

#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>

namespace terrainlab
{

const char* Node::STR_PROP_DISPLAY = "Display";

Node::Node(const std::string& title, bool props)
    : bp::Node(title)
{
}

Node::Node(const Node& node)
    : bp::Node(node)
{
    operator = (node);
}

Node& Node::operator = (const Node& node)
{
    m_name = node.m_name;

    m_bypass  = node.m_bypass;
    m_display = node.m_display;

    return *this;
}

Node::~Node()
{
}

void Node::Draw(const n2::RenderParams& rp) const
{
    bp::Node::Draw(rp);

    //if (m_preview) {
    //    auto eval = Blackboard::Instance()->GetEval();
    //    if (eval) {
    //        NodePreview::Draw(*eval, *this, rp);
    //    }
    //}
}

void Node::UpdatePins(const terraingraph::Device& node)
{
    std::vector<PinDesc> input, output;
    PortBack2Front(input, node.GetImports());
    PortBack2Front(output, node.GetExports());

    InitPins(input, output);
}

void Node::InitPins(const std::vector<PinDesc>& input,
                    const std::vector<PinDesc>& output)
{
    InitPinsImpl(input, true);
    InitPinsImpl(output, false);
    Layout();
}

void Node::InitPins(const std::string& name)
{
	rttr::type t = rttr::type::get_by_name("terraingraph::" + name);
    if (!t.is_valid()) {
        return;
    }

	rttr::variant var = t.create();
	assert(var.is_valid());

	auto method_imports = t.get_method("GetImports");
	assert(method_imports.is_valid());
	auto var_imports = method_imports.invoke(var);
	assert(var_imports.is_valid()
		&& var_imports.is_type<std::vector<terraingraph::Device::Port>>());
	auto& imports = var_imports.get_value<std::vector<terraingraph::Device::Port>>();

	auto method_exports = t.get_method("GetExports");
	assert(method_exports.is_valid());
	auto var_exports = method_exports.invoke(var);
	assert(var_exports.is_valid()
		&& var_exports.is_type<std::vector<terraingraph::Device::Port>>());
	auto& exports = var_exports.get_value<std::vector<terraingraph::Device::Port>>();

	std::vector<PinDesc> input, output;
    PortBack2Front(input, imports);
    PortBack2Front(output, exports);

	InitPins(input, output);
}

void Node::InitPinsImpl(const std::vector<PinDesc>& pins, bool is_input)
{
    auto& dst = is_input ? m_all_input : m_all_output;
    auto old_pins = dst;

	dst.clear();
	dst.reserve(pins.size());
	for (auto& d : pins)
	{
        std::shared_ptr<bp::Pin> p = nullptr;
        for (auto& old_p : old_pins) {
            if (old_p->GetName() == d.name &&
                old_p->GetType() == d.type) {
                p = old_p;
                break;
            }
        }
        if (!p) {
            p = std::make_shared<bp::Pin>(is_input, dst.size(), d.type, d.name, *this);
        }
		if (!CheckPinName(*p, dst)) {
			assert(0);
			return;
		}
		dst.push_back(p);
	}
}

void Node::PortBack2Front(std::vector<PinDesc>& dst,
                          const std::vector<terraingraph::Device::Port>& src)
{
	dst.reserve(dst.size() + src.size());
	for (int i = 0, n = src.size(); i < n; ++i)
	{
        PinDesc d;

		auto& s = src[i];
        d.type = TerrainGraph::TypeBackToFront(s.var.type);
        d.name = s.var.full_name;

        dst.push_back(d);
	}
}

}