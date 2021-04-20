/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>
#include <agribot_behavior/generic_ports/ControlSourcePort.h>

#include <agribot_common/common_enums/ControlSource.h>
#include <agribot_msgs/ToolManagerControlSource.h>

#include <ros/ros.h>
#include <ros/package.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class SwitchControlArbiters : public BT::SyncActionNode
{
 std::shared_ptr<ros::ServiceClient> tool_manager_srv_;
public:
  SwitchControlArbiters(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void init(std::shared_ptr<ros::ServiceClient> tool_service_client);
  bool changeToolControlSource(const tool_manager::ControlSource selected_source);
};

}  // namespace agribot_behavior
