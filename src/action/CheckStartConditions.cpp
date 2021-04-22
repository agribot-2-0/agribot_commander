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
  return { BT::InputPort<tool_manager::ControlSource>("condition_to_check") };
}

BT::NodeStatus CheckStartConditions::CheckStartConditions::tick()
{
  BT::Optional<tool_manager::ControlSource> msg = TreeNode::getInput<tool_manager::ControlSource>("condition_to_check");
  if (!msg)
  {
    ROS_ERROR_STREAM(
        "[BehaviorTree][CheckStartConditions] Missing required input [condition_to_check]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    switch (msg.value())
    {
      case tool_manager::ControlSource::Manual:
        return checkManualCondition();
      case tool_manager::ControlSource::Navigation:
        return checkNavigationCondition();
      default:
        return BT::NodeStatus::FAILURE;
    }
  }
}

void CheckStartConditions::initSharedFields(std::shared_ptr<Shared<std::string>> task_name,
                                            std::shared_ptr<Shared<industrial_msgs::RobotMode>> robot_mode)
{
  task_name_ = task_name;
  robot_mode_ = robot_mode;
}

inline BT::NodeStatus CheckStartConditions::checkManualCondition()
{
  return robot_mode_->getData().val == industrial_msgs::RobotMode::MANUAL ? BT::NodeStatus::SUCCESS :
                                                                            BT::NodeStatus::FAILURE;
}

inline BT::NodeStatus CheckStartConditions::checkNavigationCondition()
{
  return (task_name_->getData() != "" && robot_mode_->getData().val != industrial_msgs::RobotMode::MANUAL) ?
             BT::NodeStatus::SUCCESS :
             BT::NodeStatus::FAILURE;
}

}  // namespace agribot_behavior
