/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/LightsControl.h>

namespace agribot_behavior
{
LightsControl::LightsControl(const std::string& name, const BT::NodeConfiguration& config)
  : SyncActionNode(name, config)
{
}

BT::PortsList LightsControl::providedPorts()
{
  return { BT::InputPort<agribot_msgs::LampsControl>("lights_control") };
}

BT::NodeStatus LightsControl::tick()
{
  BT::Optional<agribot_msgs::LampsControl> msg = TreeNode::getInput<agribot_msgs::LampsControl>("lights_control");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][LightsControl] Missing required input [lights_control]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    if (!hal_lights_client_->call(msg.value()))
    {
      return BT::NodeStatus::FAILURE;
    }
  }
  return BT::NodeStatus::SUCCESS;
}

void LightsControl::initSharedFields(std::shared_ptr<ros::ServiceClient> hal_lights_client)
{
  hal_lights_client_ = hal_lights_client;
}

}  // namespace agribot_behavior
