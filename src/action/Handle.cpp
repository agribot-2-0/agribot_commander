/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/Handle.h>

namespace agribot_behavior
{
Handle::Handle(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList Handle::providedPorts()
{
  return { BT::InputPort<std::string>("action_to_handle") };
}

BT::NodeStatus Handle::tick()
{
  BT::Optional<std::string> msg = TreeNode::getInput<std::string>("action_to_handle");
  // Check if optional is valid. If not, throw its error
  if (!msg)
  {
    throw BT::RuntimeError("missing required input [action_to_handle]: ", msg.error());
  }
  std::cout << "Handle msg: " << (*msg) << std::endl;
  return BT::NodeStatus::FAILURE;
}
}  // namespace agribot_behavior
