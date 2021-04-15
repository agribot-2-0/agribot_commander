/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/SwitchControlArbiters.h>

namespace agribot_behavior
{
SwitchControlArbiters::SwitchControlArbiters(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList SwitchControlArbiters::providedPorts()
{
  return { BT::InputPort<std::string>("control_source") };
}

BT::NodeStatus SwitchControlArbiters::tick()
{
  BT::Optional<std::string> msg = TreeNode::getInput<std::string>("control_source");
  // Check if optional is valid. If not, throw its error
  if (!msg)
  {
    throw BT::RuntimeError("missing required input [control_source]: ", msg.error());
  }
  return BT::NodeStatus::SUCCESS;
}
}  // namespace agribot_behavior
