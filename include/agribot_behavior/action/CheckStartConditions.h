#pragma once

#include <behaviortree_cpp_v3/action_node.h>

namespace agribot_behavior
{
class CheckStartConditions : public BT::SyncActionNode
{
public:
  CheckStartConditions(const std::string& name);
  BT::NodeStatus tick() override;
};
}  // namespace agribot_behavior
