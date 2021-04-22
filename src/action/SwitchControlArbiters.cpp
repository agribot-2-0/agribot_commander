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
  return { BT::InputPort<tool_manager::ControlSource>("control_source") };
}

BT::NodeStatus SwitchControlArbiters::tick()
{
  BT::Optional<tool_manager::ControlSource> msg = TreeNode::getInput<tool_manager::ControlSource>("control_source");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiters] Missing required input [control_source]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    if (changeToolControlSource(msg.value()))
    {
      ROS_INFO_STREAM("[BehaviorTree][SwitchControlArbiters] Successfuly chagned tools control source");
    }
    else
    {
      ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiters] Failed to change tool control source");
      return BT::NodeStatus::FAILURE;
    }
    changeCmdVelControlSource(tool_manager::ControlSource::None);
    if (changeCmdVelControlSource(msg.value()))
    {
      ROS_INFO_STREAM("[BehaviorTree][SwitchControlArbiters] Successfuly chagned cmd_vel control source");
    }
    else
    {
      changeToolControlSource(tool_manager::ControlSource::None);
      ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiters] Failed to change cmd_vel control source");
      return BT::NodeStatus::FAILURE;
    }
  }
  return BT::NodeStatus::SUCCESS;
}

void SwitchControlArbiters::initToolArbiterClient(std::shared_ptr<ros::ServiceClient> tool_service_client)
{
  tool_arbiter_client_ = tool_service_client;
}

void SwitchControlArbiters::initCmdVelArbiterClients(std::shared_ptr<ros::ServiceClient> cmd_vel_manual,
                                                     std::shared_ptr<ros::ServiceClient> cmd_vel_navigation,
                                                     std::shared_ptr<ros::ServiceClient> cmd_vel_stop)
{
  cmd_vel_manual_client_ = cmd_vel_manual;
  cmd_vel_navigation_client_ = cmd_vel_navigation;
  cmd_vel_stop_client_ = cmd_vel_stop;
}

bool SwitchControlArbiters::changeToolControlSource(const tool_manager::ControlSource selected_source)
{
  agribot_msgs::ToolManagerControlSource srv;
  srv.request.source = static_cast<std::uint8_t>(selected_source);
  if (!tool_arbiter_client_->call(srv))
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiters] Failed to call tool control service");
    return false;
  }
  return (srv.response.result == srv.response.SUCCESS ? true : false);
}

bool SwitchControlArbiters::changeCmdVelControlSource(const tool_manager::ControlSource selected_source)
{
  std_srvs::Empty srv;
  bool success = false;
  switch (selected_source)
  {
    case tool_manager::ControlSource::Manual:
      success = cmd_vel_manual_client_->call(srv);
      break;
    case tool_manager::ControlSource::Navigation:
      success = cmd_vel_navigation_client_->call(srv);
      break;
    case tool_manager::ControlSource::None:
      success = cmd_vel_stop_client_->call(srv);
      break;
    default:
      break;
  }
  if (!success)
  {
    ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiters] Failed to call cmd_vel control service");
  }
  return success;
}

}  // namespace agribot_behavior
