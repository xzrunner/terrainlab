#pragma once

#include <blueprint/typedef.h>

#include <terraingraph/Evaluator.h>
#include <terraingraph/typedef.h>
#include <node0/typedef.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <unordered_map>

namespace bp { class Connecting; }

namespace terrainlab
{

class Evaluator : boost::noncopyable
{
public:
    Evaluator();

    // update nodes
    void OnAddNode(const bp::Node& node, const n0::SceneNodePtr& snode,
        bool need_update = true);
    void OnRemoveNode(const bp::Node& node);
    void OnClearAllNodes();

    // update node prop
    void OnNodePropChanged(const bp::NodePtr& node);

    // update node conn
    void OnConnected(const bp::Connecting& conn);
    void OnDisconnecting(const bp::Connecting& conn);
    void OnRebuildConnection();

    auto& GetEval() const { return m_eval; }

    terraingraph::DevicePtr  QueryBackNode(const bp::Node& front_node) const;

private:
    void Update();

private:
    terraingraph::Evaluator m_eval;

    std::unordered_map<const bp::Node*, terraingraph::DevicePtr> m_front2back;

}; // Evaluator

}