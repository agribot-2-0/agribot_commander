/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/common/SharedField.h>
#include <agribot_behavior/generic_ports/LightsControlPort.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <ros/console.h>
#include <ros/ros.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class LightsControl : public BT::SyncActionNode
{
private:
  std::shared_ptr<ros::ServiceClient> hal_lights_client_;

public:
  LightsControl(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initSharedFields(std::shared_ptr<ros::ServiceClient> hal_lights_client);
};
}  // namespace agribot_behavior
