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
    ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiter] Missing required input [control_source]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    if (!changeToolControlSource(msg.value()))
    {
      ROS_ERROR_STREAM("[BehaviorTree][SwitchControlArbiter] Failed to change state");
      return BT::NodeStatus::FAILURE;
    }
  }
  return BT::NodeStatus::SUCCESS;
}

void SwitchControlArbiters::init(std::shared_ptr<ros::ServiceClient> tool_service_client)
{
  tool_manager_srv_ = tool_service_client;
}

bool SwitchControlArbiters::changeToolControlSource(const tool_manager::ControlSource selected_source)
{
  agribot_msgs::ToolManagerControlSource srv;
  srv.request.source = static_cast<std::uint8_t>(selected_source);
  if (!tool_manager_srv_->call(srv))
  {
    ROS_ERROR("Failed to call service");
  }
  return srv.response.result == srv.response.SUCCESS ? true : false;
}

}  // namespace agribot_behavior
