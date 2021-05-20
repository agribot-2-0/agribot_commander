/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/SwitchTtcontrolState.h>

namespace agribot_behavior
{
SwitchTtcontrolState::SwitchTtcontrolState(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList SwitchTtcontrolState::providedPorts()
{
  return { BT::InputPort<agribot_behavior::TtcontrolState>("ttcontrol_state") };
}

BT::NodeStatus SwitchTtcontrolState::tick()
{
  BT::Optional<agribot_behavior::TtcontrolState> msg =
      TreeNode::getInput<agribot_behavior::TtcontrolState>("ttcontrol_state");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchTtcontrolState] Missing required input [ttcontrol_state]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    switch (msg.value())
    {
      case agribot_behavior::TtcontrolState::SafeState:
        return switchToSafeState();
      case agribot_behavior::TtcontrolState::Autonomous:
        return switchToAutonomous();
      default:
        return BT::NodeStatus::FAILURE;
    }
  }
  return BT::NodeStatus::FAILURE;
}

void SwitchTtcontrolState::initTtcontrolStateClients(std::shared_ptr<ros::ServiceClient> ttcontrol_safe_state_client,
                                                     std::shared_ptr<ros::ServiceClient> ttcontrol_auto_state_client)
{
  ttcontrol_safe_state_client_ = ttcontrol_safe_state_client;
  ttcontrol_auto_state_client_ = ttcontrol_auto_state_client;
}

BT::NodeStatus SwitchTtcontrolState::switchToSafeState()
{
  std_srvs::Trigger trigger;
  if (!ttcontrol_safe_state_client_->call(trigger))
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchTtcontrolState] Failed to switch TTControl state to safe state");
    return BT::NodeStatus::FAILURE;
  }
  ROS_WARN_STREAM("[BehaviorTree][SwitchTtcontrolState] TTControl state changed to safe state");
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SwitchTtcontrolState::switchToAutonomous()
{
  std_srvs::Trigger trigger;
  if (!ttcontrol_auto_state_client_->call(trigger))
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchTtcontrolState] Failed to switch TTControl state to auto. Service error");
    return BT::NodeStatus::FAILURE;
  }
  else if (trigger.response.success == false)
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchTtcontrolState] Failed to switch TTControl state to auto");
    ROS_ERROR_STREAM("[BehaviorTree][SwitchTtcontrolState] Enter safe state from mobile app first");
    return BT::NodeStatus::FAILURE;
  }
  ROS_WARN_STREAM("[BehaviorTree][SwitchTtcontrolState] TTControl state changed state to auto");
  return BT::NodeStatus::SUCCESS;
}

}  // namespace agribot_behavior
