/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/generic_ports/ControlEnginePort.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <ros/console.h>
#include <ros/ros.h>

#include <std_srvs/Trigger.h>

#include <memory>
#include <string>

namespace agribot_behavior
{
class ControlEngine : public BT::SyncActionNode
{
private:
  std::shared_ptr<ros::ServiceClient> start_engine_client_;
  std::shared_ptr<ros::ServiceClient> stop_engine_client_;

  BT::NodeStatus startEngine();
  BT::NodeStatus stopEngine();

public:
  ControlEngine(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initEngineControlClients(std::shared_ptr<ros::ServiceClient> start_engine_client,
                                std::shared_ptr<ros::ServiceClient> stop_engine_client);
};
}  // namespace agribot_behavior
