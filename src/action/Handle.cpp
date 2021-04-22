/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/Handle.h>


namespace agribot_behavior
{
Handle::Handle(const std::string& name, const BT::NodeConfiguration& config) : AsyncActionNode(name, config)
{
  halt_requested_.store(false);
}

BT::PortsList Handle::providedPorts()
{
  return { BT::InputPort<tool_manager::ControlSource>("action_to_handle") };
}

BT::NodeStatus Handle::tick()
{
  BT::Optional<tool_manager::ControlSource> msg = TreeNode::getInput<tool_manager::ControlSource>("action_to_handle");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][Handle] Missing required input [action_to_handle]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    while (!halt_requested_)
    {
      ROS_WARN_STREAM("[BehaviorTree][Handle] Running ");
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    halt_requested_.store(false);
    return BT::NodeStatus::FAILURE;
  }
}

void Handle::halt()
{
  halt_requested_.store(true);
}

}  // namespace agribot_behavior
