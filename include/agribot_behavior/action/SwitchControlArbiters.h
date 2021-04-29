/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>
#include <agribot_behavior/generic_ports/ControlSourcePort.h>

#include <agribot_common/common_enums/ControlSource.h>

#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <agribot_msgs/ToolManagerControlSource.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class SwitchControlArbiters : public BT::SyncActionNode
{
  std::shared_ptr<ros::ServiceClient> tool_arbiter_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_manual_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_navigation_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_stop_client_;

public:
  SwitchControlArbiters(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initToolArbiterClient(std::shared_ptr<ros::ServiceClient> tool_service_client);
  void initCmdVelArbiterClients(std::shared_ptr<ros::ServiceClient> cmd_vel_manual,
                                std::shared_ptr<ros::ServiceClient> cmd_vel_navigation,
                                std::shared_ptr<ros::ServiceClient> cmd_vel_stop);
  bool changeToolControlSource(const tool_manager::ControlSource selected_source);
  bool changeCmdVelControlSource(const tool_manager::ControlSource selected_source);
};

}  // namespace agribot_behavior
