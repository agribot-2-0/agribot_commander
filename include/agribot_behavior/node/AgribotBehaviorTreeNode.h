/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>

#include <agribot_behavior/action/CheckStartConditions.h>
#include <agribot_behavior/action/SwitchControlArbiters.h>
#include <agribot_behavior/action/Handle.h>
#include <agribot_behavior/common/SharedField.h>
#include <agribot_behavior/common/OneTimeLogger.h>
#include <agribot_behavior/path_handle/PathFeeder.h>
#include <agribot_common/CommonFunctions/ros_common_functions.h>

#include <ros/ros.h>
#include <ros/package.h>

#include <std_srvs/Empty.h>
#include <std_msgs/String.h>
#include <industrial_msgs/RobotMode.h>
#include <agribot_msgs/ToolManagerControlSource.h>
#include <agribot_msgs/AgribotMoveBaseCommand.h>
#include <agribot_msgs/AgribotNavigationManagerResult.h>
#include <agribot_msgs/AgribotObstacleDetection.h>
#include <agribot_msgs/NavSatFixOrientation.h>
#include <agribot_msgs/AgribotAppErrorFlag.h>

#include <experimental/filesystem>  // when migrating to GCC version >= 8.0 experimental part of header should be removed
#include <string>
#include <atomic>
#include <memory>

namespace agribot_behavior
{
class AgribotBehaviorTreeNode
{
private:
  enum class CheckAbsenceOf
  {
    Obstacle,
    MobileAppErrorFlag
  };

  std::unique_ptr<BT::BehaviorTreeFactory> factory_;
  std::unique_ptr<BT::Tree> tree_;
  OneTimeLogger log_once_;

  std::experimental::filesystem::path task_folder_;
  std::experimental::filesystem::path path_folder_;

  std::shared_ptr<ros::NodeHandle> node_handle_;
  std::unique_ptr<BT::PublisherZMQ> tree_state_publisher_;  // used by Groot

  // Publishers - initialized inside AgribotBehaviorTreeNode. The tree nodes, which need to publish something are
  // granted with shared_ptr of the Publishers
  std::shared_ptr<ros::Publisher> move_base_command_publisher_;
  std::shared_ptr<ros::Publisher> waypoints_publisher_;

  // Subscribers - initialized inside AgribotBehaviorTreeNode. Also the callbacks are defined in
  // AgribotBehaviorTreeNode. The callbacks are thread-safe and are distributing ros messages to proper class fields.
  // The tree nodes, which need access to the fileds, are granted with shared_ptr to desired field
  ros::Subscriber task_name_subscriber_;
  ros::Subscriber robot_mode_subscriber_;
  ros::Subscriber move_base_result_subscriber_;
  ros::Subscriber obstacle_detected_subscriber_;
  ros::Subscriber mobile_app_error_flag_subscriber_;

  // Services - initialized inside AgribotBehaviorTreeNode. The tree nodes, which need to call service are
  // granted with shared_ptr of the ServiceClient. The implementation of service call is stored inside individual tree
  // node
  std::shared_ptr<ros::ServiceClient> tool_arbiter_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_manual_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_navigation_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_stop_client_;

  // Variables shared between tree nodes, which are changed in callbacks. The thread safety is guaranteed by Shared<>
  // class
  std::shared_ptr<SharedField<std::string>> task_name_;
  std::shared_ptr<SharedField<industrial_msgs::RobotMode>> robot_mode_;
  std::shared_ptr<SharedField<agribot_msgs::AgribotNavigationManagerResult>> navigation_manager_result_;

  std::atomic<bool> obstacle_absent_;
  std::atomic<bool> error_absent_;

  BT::NodeStatus simpleConditionCheck(const CheckAbsenceOf check);
  void registerTreeNodes();
  std::string getTreeLocation() const;
  void initTreeNodesWithRosDependencies();
  void initSharedData();
  void initAtomics();
  void initRosPublishers();
  void initRosSubscribers();
  void initRosServices();

  void taskNameRosCallback(const std_msgs::String::ConstPtr& msg);
  void robotModeRosCallback(const industrial_msgs::RobotMode::ConstPtr& msg);
  void NavigationManagerResultRosCallback(const agribot_msgs::AgribotNavigationManagerResult::ConstPtr& msg);
  void obstacleDetectedRosCallback(const agribot_msgs::AgribotObstacleDetection::ConstPtr& msg);
  void mobileAppErrorFlagRosCallback(const agribot_msgs::AgribotAppErrorFlag::ConstPtr& msg);

public:
  AgribotBehaviorTreeNode(int argc, char** argv);
  void run();
};
}  // namespace agribot_behavior
