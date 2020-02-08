#include "wmv/Evaluator.h"
#include "wmv/WmAdapter.h"
#include "wmv/Node.h"

#include <blueprint/Node.h>
#include <blueprint/Pin.h>
#include <blueprint/Connecting.h>
#include <blueprint/CompNode.h>

#include <wm/Device.h>
#include <wm/Evaluator.h>

#include <queue>

namespace wmv
{

Evaluator::Evaluator()
{
}

void Evaluator::OnAddNode(const bp::Node& front, const n0::SceneNodePtr& snode, bool need_update)
{
    auto back = WmAdapter::CreateBackFromFront(front);
    if (!back) {
        return;
    }

    m_eval.AddDevice(back);

    m_front2back.insert({ &front, back });

    if (front.get_type().is_derived_from<Node>()) {
        const_cast<Node&>(static_cast<const Node&>(front)).SetName(back->GetName());
    }

    WmAdapter::UpdatePropBackFromFront(front, *back, *this);
    if (need_update) {
        Update();
    }
}

void Evaluator::OnRemoveNode(const bp::Node& node)
{
    auto itr = m_front2back.find(&node);
    if (itr == m_front2back.end()) {
        return;
    }

    m_eval.RemoveDevice(itr->second);
    m_front2back.erase(itr);

    Update();
}

void Evaluator::OnClearAllNodes()
{
    m_eval.ClearAllDevices();
    m_front2back.clear();

    Update();
}

void Evaluator::OnNodePropChanged(const bp::NodePtr& node)
{
    auto itr = m_front2back.find(node.get());
    // not wm node
    if (itr == m_front2back.end()) {
        return;
    }

    WmAdapter::UpdatePropBackFromFront(*node, *itr->second, *this);

    if (node->get_type().is_derived_from<Node>())
    {
        auto& terrv_n = std::static_pointer_cast<Node>(node);
        if (terrv_n->GetName() != itr->second->GetName()) {
            //m_eval.Rename(itr->second->GetName(), terrv_n->GetName());
            if (itr->second->GetName() != terrv_n->GetName()) {
                terrv_n->SetName(itr->second->GetName());
            }
        }
    }

    //m_eval.MakeDirty();

    Update();
}

void Evaluator::OnConnected(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_front2back.find(&f_pin->GetParent());
    auto t_itr = m_front2back.find(&t_pin->GetParent());
    if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
        return;
    }

    //if (t_itr->first->GetAllInput().size() > t_itr->second->GetImports().size()) {
    //    t_itr->second->AddInputPorts(t_itr->first->GetAllInput().size() - t_itr->first->GetAllOutput().size());
    //}

    m_eval.Connect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    Update();
}

void Evaluator::OnDisconnecting(const bp::Connecting& conn)
{
    auto f_pin = conn.GetFrom();
    auto t_pin = conn.GetTo();

    auto f_itr = m_front2back.find(&f_pin->GetParent());
    auto t_itr = m_front2back.find(&t_pin->GetParent());
    if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
        return;
    }

    m_eval.Disconnect(
        { f_itr->second, f_pin->GetPosIdx() },
        { t_itr->second, t_pin->GetPosIdx() }
    );

    Update();
}

void Evaluator::OnRebuildConnection()
{
    std::vector<std::pair<wm::Device::PortAddr, wm::Device::PortAddr>> conns;
    for (auto& itr : m_front2back)
    {
        auto& front = itr.first;
        auto& back  = itr.second;
        for (auto& in : front->GetAllInput())
        {
            for (auto& conn : in->GetConnecting())
            {
                auto f_pin = conn->GetFrom();
                auto t_pin = conn->GetTo();

                auto f_itr = m_front2back.find(&f_pin->GetParent());
                auto t_itr = m_front2back.find(&t_pin->GetParent());
                if (f_itr == m_front2back.end() || t_itr == m_front2back.end()) {
                    continue;
                }

                //if (t_itr->first->GetAllInput().size() > t_itr->second->GetImports().size()) {
                //    t_itr->second->AddInputPorts(t_itr->first->GetAllInput().size() - t_itr->first->GetAllOutput().size());
                //}

                conns.push_back({
                    { f_itr->second, f_pin->GetPosIdx() },
                    { t_itr->second, t_pin->GetPosIdx() }
                });
            }
        }
    }

    m_eval.RebuildConnections(conns);

    Update();
}

wm::DevicePtr Evaluator::QueryBackNode(const bp::Node& front_node) const
{
    auto itr = m_front2back.find(&front_node);
    return itr == m_front2back.end() ? nullptr : itr->second;
}

void Evaluator::Update()
{
    m_eval.Update();
}

}