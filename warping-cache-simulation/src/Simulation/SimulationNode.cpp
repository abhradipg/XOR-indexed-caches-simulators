#include "SimulationNode.hpp"

#include <utility>

SimulationNode::SimulationNode(NodeId nodeId) : nodeId(std::move(nodeId)) {}

SimulationNode::SimulationNode(NodeId nodeId, const isl::set &iterationDomain)
    : nodeId(std::move(nodeId)), iterationDomain(iterationDomain) {}

const NodeId &SimulationNode::GetNodeId() const { return this->nodeId; }

const isl::set &SimulationNode::GetIterationDomain() const {
  return this->iterationDomain;
}
