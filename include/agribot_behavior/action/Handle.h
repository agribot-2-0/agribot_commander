/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>

#include <agribot_behavior/path_handle/PathFeeder.h>
#include <agribot_behavior/common/Shared.h>
#include <agribot_common/common_enums/ControlSource.h>

#include <ros/ros.h>
#include <ros/console.h>
#include <agribot_msgs/NavSatFixOrientation.h>
#include <agribot_msgs/AgribotMoveBaseCommand.h>

#include <memory>
#include <atomic>
#include <string>

namespace agribot_behavior
{
class Handle : public BT::AsyncActionNode
{
private:
  PathFeeder path_feeder_;

  std::shared_ptr<Shared<std::string>> task_name_;
  std::shared_ptr<Shared<agribot_msgs::AgribotMoveBaseMonitorResult>> move_base_monitor_result_;

  std::shared_ptr<ros::Publisher> waypoints_publisher_;
  std::shared_ptr<ros::Publisher> move_base_command_publisher_;
  std::atomic<bool> halt_requested_;

public:
  Handle(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void initSharedFields(std::shared_ptr<Shared<std::string>> task_name,
                        std::shared_ptr<Shared<agribot_msgs::AgribotMoveBaseMonitorResult>> move_base_monitor_result);
  void initPublishers(std::shared_ptr<ros::Publisher> waypoints_publisher,
                      std::shared_ptr<ros::Publisher> move_base_command_publisher);
  void halt() override;
  void handleManualControl();
  void handleAutonomousControl();
  void publishWaypoint(const agribot_msgs::NavSatFixOrientation& waypoint) const;
  void publishMoveBaseCommand(const uint8_t command) const;
  void resetAutonomousDriving();
  bool moveBaseActive();
  bool moveBaseReady();
};
}  // namespace agribot_behavior
