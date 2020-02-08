#include "wmv/WMV.h"
#include "wmv/PinCallback.h"
#include "wmv/Node.h"

#include <blueprint/NodeBuilder.h>
#include <blueprint/node/Commentary.h>

#include <wm/WM.h>

namespace wmv
{

CU_SINGLETON_DEFINITION(WMV);

extern void regist_rttr();

WMV::WMV()
{
	wm::WM::Instance();

	regist_rttr();

	InitNodes();

    InitPinCallback();
}

void WMV::InitNodes()
{
    const int bp_count = 1;

	auto list = rttr::type::get<Node>().get_derived_classes();
	m_nodes.reserve(bp_count + list.size());

    m_nodes.push_back(std::make_shared<bp::node::Commentary>());

	for (auto& t : list)
	{
		auto obj = t.create();
		assert(obj.is_valid());
		auto node = obj.get_value<bp::NodePtr>();
		assert(node);
		m_nodes.push_back(node);
	}
}

}