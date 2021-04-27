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
    switch (msg.value())
    {
      case tool_manager::ControlSource::Manual:
        ROS_WARN_STREAM("[BehaviorTree][Handle] Running manual action");
        handleManualControl();
        break;
      case tool_manager::ControlSource::Navigation:
        ROS_WARN_STREAM("[BehaviorTree][Handle] Running autonomous action");
        handleAutonomousControl();
        break;
      default:
        break;
    }
    halt_requested_.store(false);
    return BT::NodeStatus::FAILURE;
  }
}

void Handle::initSharedFields(std::shared_ptr<Shared<std::string>> task_name)
{
  task_name_ = task_name;
}

void Handle::halt()
{
  halt_requested_.store(true);
}

// Manual control is idle action since another ROS nodes are handling it
void Handle::handleManualControl()
{
  while (!halt_requested_)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

void Handle::handleAutonomousControl()
{
  if (path_feeder_.loadPath(task_name_->getData()))
  {
    ROS_WARN_STREAM("[BehaviorTree][Handle] Loaded path " << path_feeder_.getLoadedPathLocation());
    while (!halt_requested_)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
  else
  {
    ROS_ERROR_STREAM("[BehaviorTree][Handle] Failed to load path from task " << task_name_->getData());
  }
}

}  // namespace agribot_behavior
