/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/common/SharedField.h>
#include <agribot_behavior/generic_ports/TtcontrolStatePort.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <ros/console.h>
#include <ros/ros.h>

#include <std_srvs/Trigger.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class SwitchTtcontrolState : public BT::SyncActionNode
{
private:
  std::shared_ptr<ros::ServiceClient> ttcontrol_safe_state_client_;
  std::shared_ptr<ros::ServiceClient> ttcontrol_auto_state_client_;

  BT::NodeStatus switchToSafeState();
  BT::NodeStatus switchToAutonomous();

public:
  SwitchTtcontrolState(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initTtcontrolStateClients(std::shared_ptr<ros::ServiceClient> ttcontrol_safe_state_client,
                                 std::shared_ptr<ros::ServiceClient> ttcontrol_auto_state_client);
};
}  // namespace agribot_behavior
