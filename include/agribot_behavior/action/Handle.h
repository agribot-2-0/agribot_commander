/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/action_node.h>

#include <agribot_behavior/path_handle/PathParser.h>
#include <agribot_behavior/common/SharedField.h>
#include <agribot_common/common_enums/ControlSource.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <actionlib/client/simple_action_client.h>
#pragma GCC diagnostic pop
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/NavSatFix.h>
#include <agribot_msgs/AgribotMoveBaseCommand.h>
#include <agribot_msgs/PerformAutonomousPathAction.h>

#include <memory>
#include <atomic>
#include <string>
#include <vector>

namespace agribot_behavior
{
class Handle : public BT::AsyncActionNode
{
private:
  std::unique_ptr<PathParser> path_parser_;

  std::shared_ptr<SharedField<std::string>> task_name_;

  std::shared_ptr<ros::Publisher> move_base_command_publisher_;
  std::shared_ptr<actionlib::SimpleActionClient<agribot_msgs::PerformAutonomousPathAction>>
      autonomous_path_action_client_;
  agribot_msgs::AgribotNavigationManagerResult autonomous_path_action_feedback_;

  std::atomic<bool> halt_requested_;

  void handleManualControl();
  void handleAutonomousControl();
  void publishMoveBaseCommand(const uint8_t command) const;
  void resetAutonomousDriving();
  bool moveBaseActive() const;
  bool moveBaseReady() const;

  void sendAutonomousPathGoal(const std::vector<sensor_msgs::NavSatFix>& path_points);

public:
  Handle(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
  void halt() override;
  void initPathParser(const std::string& path_folder, const std::string& task_folder);
  void initSharedFields(std::shared_ptr<SharedField<std::string>> task_name);
  void initPublishers(std::shared_ptr<ros::Publisher> move_base_command_publisher);
  void initActionlibs(std::shared_ptr<actionlib::SimpleActionClient<agribot_msgs::PerformAutonomousPathAction>>
                          autonomous_path_action_client);
};
}  // namespace agribot_behavior
