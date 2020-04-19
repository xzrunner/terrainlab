#include "terrainlab/Node.h"
#include "terrainlab/PinType.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <terraingraph/Device.h>

namespace terrainlab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::Draw(const ur2::Device& dev, ur2::Context& ctx,
                const n2::RenderParams& rp) const
{
    bp::Node::Draw(dev, ctx, rp);

    //if (m_preview) {
    //    auto eval = Blackboard::Instance()->GetEval();
    //    if (eval) {
    //        NodePreview::Draw(*eval, *this, rp);
    //    }
    //}
}

void Node::InitPins(const std::string& name)
{
    auto back2front = [](const dag::Node<terraingraph::DeviceVarType>::Port& back) -> bp::PinDesc
    {
        bp::PinDesc front;

        switch (back.var.type)
        {
        case terraingraph::DeviceVarType::Any:
            front.type = bp::PIN_ANY_VAR;
            break;
        case terraingraph::DeviceVarType::Heightfield:
            front.type = PIN_HEIGHTFIELD;
            break;
        case terraingraph::DeviceVarType::Bitmap:
            front.type = PIN_BITMAP;
            break;
        case terraingraph::DeviceVarType::Mask:
            front.type = PIN_MASK;
            break;
        default:
            assert(0);
        }

        front.name = back.var.full_name;

        return front;
    };

    bp::BackendAdapter<terraingraph::DeviceVarType>
        trans("terraingraph", back2front);
    trans.InitNodePins(*this, name);
}

}