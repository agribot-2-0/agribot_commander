/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/CheckStartConditions.h>

namespace agribot_behavior
{
CheckStartConditions::CheckStartConditions(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList CheckStartConditions::providedPorts()
{
  return { BT::InputPort<std::string>("action_to_be_started") };
}

BT::NodeStatus CheckStartConditions::CheckStartConditions::tick()
{
  BT::Optional<std::string> msg = TreeNode::getInput<std::string>("action_to_be_started");
  // Check if optional is valid. If not, throw its error
  if (!msg)
  {
    throw BT::RuntimeError("missing required input [action_to_be_started]: ", msg.error());
  }
  return BT::NodeStatus::SUCCESS;
}
}  // namespace agribot_behavior
