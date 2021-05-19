/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_behavior/action/CheckStartConditions.h>
#include <agribot_behavior/action/Handle.h>
#include <agribot_behavior/action/LightsControl.h>
#include <agribot_behavior/action/SwitchControlArbiters.h>
#include <agribot_behavior/action/SwitchTtcontrolState.h>
#include <agribot_behavior/action/ControlEngine.h>
#include <agribot_behavior/common/OneTimeLogger.h>
#include <agribot_common/CommonFunctions/ros_common_functions.h>

#include <ros/ros.h>
#include <ros/package.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <actionlib/client/simple_action_client.h>
#include <actionlib/server/simple_action_server.h>
#pragma GCC diagnostic pop

#include <std_srvs/Empty.h>
#include <std_msgs/String.h>
#include <std_srvs/Trigger.h>
#include <industrial_msgs/RobotMode.h>
#include <agribot_msgs/LampsControl.h>
#include <agribot_behavior/common/SharedField.h>
#include <agribot_msgs/PerformAutonomousPathAction.h>
#include <agribot_msgs/PerformAutonomousTaskAction.h>
#include <agribot_msgs/ToolManagerControlSource.h>
#include <agribot_msgs/AgribotNavigationManagerResult.h>
#include <agribot_msgs/AgribotObstacleDetection.h>
#include <agribot_msgs/AgribotTtcontrolState.h>
#include <agribot_msgs/AgribotAppErrorFlag.h>
#include <sensor_msgs/NavSatFix.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/loggers/bt_zmq_publisher.h>

#include <atomic>
// when migrating to GCC version >= 8.0 experimental part of header should be removed
#include <experimental/filesystem>  //NOLINT (build/include_order)
#include <memory>
#include <string>

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

  // ActionlibClient - initialized inside AgribotBehaviorTreeNode. The tree nodes, which need to set goal for actionlib
  // are granted with shared_ptr of the SimpleActionClient
  std::shared_ptr<actionlib::SimpleActionClient<agribot_msgs::PerformAutonomousPathAction>>
      autonomous_path_action_client_;

  // ActionlibServer is used only in AgribotBehaviorTreeNode, and is not shared with other tree nodes
  std::unique_ptr<actionlib::SimpleActionServer<agribot_msgs::PerformAutonomousTaskAction>>
      autonomous_task_action_server_;

  // Subscribers - initialized inside AgribotBehaviorTreeNode. Also the callbacks are defined in
  // AgribotBehaviorTreeNode. The callbacks are thread-safe and are distributing ros messages to proper class fields.
  // The tree nodes, which need access to the fileds, are granted with shared_ptr to desired field
  ros::Subscriber obstacle_detected_subscriber_;
  ros::Subscriber mobile_app_error_flag_subscriber_;
  ros::Subscriber ttcontrol_state_subscriber_;

  // Publishers - initialized inside AgribotBehaviorTreeNode. The tree nodes, which need to publish something are
  // granted with shared_ptr of the Publishers
  std::shared_ptr<ros::Publisher> robot_mode_publisher_;

  // Services - initialized inside AgribotBehaviorTreeNode. The tree nodes, which need to call service are
  // granted with shared_ptr of the ServiceClient. The implementation of service call is stored inside individual tree
  // node
  std::shared_ptr<ros::ServiceClient> tool_arbiter_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_manual_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_navigation_client_;
  std::shared_ptr<ros::ServiceClient> cmd_vel_stop_client_;
  std::shared_ptr<ros::ServiceClient> hal_lights_client_;
  std::shared_ptr<ros::ServiceClient> ttcontrol_safe_state_client_;
  std::shared_ptr<ros::ServiceClient> ttcontrol_auto_state_client_;
  std::shared_ptr<ros::ServiceClient> engine_start_client_;
  std::shared_ptr<ros::ServiceClient> engine_stop_client_;

  // Variables shared between tree nodes, which are changed in callbacks. The thread safety is guaranteed by Shared<>
  // class
  std::shared_ptr<SharedField<std::string>> current_task_name_;
  std::shared_ptr<SharedField<std::string>> previous_task_name_;
  std::shared_ptr<SharedField<std::size_t>> path_progression_;
  std::shared_ptr<SharedField<industrial_msgs::RobotMode>> robot_mode_;
  std::shared_ptr<SharedField<agribot_msgs::AgribotNavigationManagerResult>> autonomous_path_action_feedback_;
  std::shared_ptr<SharedField<agribot_msgs::AgribotTtcontrolState>> ttcontrol_state_;

  std::atomic<bool> obstacle_absent_;
  std::atomic<bool> error_absent_;

  BT::NodeStatus simpleConditionCheck(const CheckAbsenceOf check);
  void registerTreeNodes();
  std::string getTreeLocation() const;
  void initTreeNodesWithRosDependencies();
  void initSharedData();
  void initAtomics();
  void initRosActionlibClients();
  void initRosActionlibServers();
  void initRosSubscribers();
  void initRosPublishers();
  void initRosServices();

  void taskNameRosCallback(const std_msgs::String::ConstPtr& msg);
  void robotModeRosCallback(const industrial_msgs::RobotMode::ConstPtr& msg);
  void obstacleDetectedRosCallback(const agribot_msgs::AgribotObstacleDetection::ConstPtr& msg);
  void mobileAppErrorFlagRosCallback(const agribot_msgs::AgribotAppErrorFlag::ConstPtr& msg);
  void ttcontrolStateCallback(const agribot_msgs::AgribotTtcontrolState::ConstPtr& msg);

  void performAutonomousTaskCallback(const agribot_msgs::PerformAutonomousTaskGoalConstPtr& goal);
  bool pathFinished();

public:
  AgribotBehaviorTreeNode(int argc, char** argv);
  void run();
};
}  // namespace agribot_behavior
