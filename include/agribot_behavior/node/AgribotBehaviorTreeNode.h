/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>
#include <agribot_behavior/action/CheckStartConditions.h>
#include <agribot_behavior/action/SwitchControlArbiters.h>
#include <agribot_behavior/action/Handle.h>
#include <agribot_behavior/common/Shared.h>
#include <agribot_behavior/path_handle/PathFeeder.h>

#include <ros/ros.h>
#include <ros/package.h>

#include <std_srvs/Empty.h>
#include <std_msgs/String.h>
#include <industrial_msgs/RobotMode.h>
#include <agribot_msgs/ToolManagerControlSource.h>
#include <agribot_msgs/AgribotMoveBaseCommand.h>
#include <agribot_msgs/AgribotMoveBaseMonitorResult.h>
#include <agribot_msgs/AgribotObstacleDetection.h>
#include <agribot_msgs/NavSatFixOrientation.h>

#include <string>
#include <memory>

namespace agribot_behavior
{
class AgribotBehaviorTreeNode
{
private:
  std::unique_ptr<BT::BehaviorTreeFactory> factory_;
  std::unique_ptr<BT::Tree> tree_;

  std::shared_ptr<ros::NodeHandle> node_handle_;
  std::unique_ptr<BT::PublisherZMQ> tree_state_publisher_;  // used by Groot

  // Publishers
  std::shared_ptr<ros::Publisher> move_base_command_publisher_;
  std::shared_ptr<ros::Publisher> waypoints_publisher_;
  // Subscribers
  ros::Subscriber task_name_subscriber_;
  ros::Subscriber robot_mode_subscriber_;
  ros::Subscriber move_base_result_subscriber_;
  ros::Subscriber obstacle_detected_subscriber_;
  // Parameters
  // Services
  std::shared_ptr<ros::ServiceClient> tool_arbiter_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_manual_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_navigation_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_stop_client_;

  // Variables shared between tree nodes, which are changed in callbacks
  std::shared_ptr<Shared<std::string>> task_name_;
  std::shared_ptr<Shared<industrial_msgs::RobotMode>> robot_mode_;
  std::shared_ptr<Shared<agribot_msgs::AgribotMoveBaseMonitorResult>> move_base_monitor_result_;

public:
  AgribotBehaviorTreeNode(int argc, char** argv);
  void registerAllNodes();
  std::string getTreeLocation() const;
  void initTreeNodesWithRosDependencies();
  void initSharedData();
  void run();
  void taskNameCallback(const std_msgs::String::ConstPtr& msg);
  void robotModeCallback(const industrial_msgs::RobotMode::ConstPtr& msg);
  void moveBaseMonitorResultCallback(const agribot_msgs::AgribotMoveBaseMonitorResult::ConstPtr& msg);
  void obstacleDetectedCallback(const agribot_msgs::AgribotObstacleDetection::ConstPtr& msg);
  BT::NodeStatus checkObstacle();
};
}  // namespace agribot_behavior
