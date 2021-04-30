/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

namespace agribot_behavior
{
typedef ros::console::Level RosLogLevel;

BT::NodeStatus AgribotBehaviorTreeNode::simpleCheck(ToCheck check)
{
  switch (check)
  {
    case ToCheck::mobile_app_error_flag:
      if (error_absent_.load())
      {
        log_once_.reset("mobile_app_error");
        return BT::NodeStatus::SUCCESS;
      }
      log_once_.log("mobile_app_error", RosLogLevel::Warn, "[AgribotBehavior] MobileAppError detected");
      break;
    case ToCheck::obstacle:
      if (obstacle_absent_.load())
      {
        log_once_.reset("obstacle");
        return BT::NodeStatus::SUCCESS;
      }
      log_once_.log("obstacle", RosLogLevel::Warn, "[AgribotBehavior] Obstacle detected");
      break;
    default:
      break;
  }

  return BT::NodeStatus::FAILURE;
}

void AgribotBehaviorTreeNode::registerNodes()
{
  factory_->registerNodeType<CheckStartConditions>("CheckStartConditions");
  factory_->registerNodeType<SwitchControlArbiters>("SwitchControlArbiters");
  factory_->registerNodeType<Handle>("Handle");
  factory_->registerSimpleCondition("ObstacleAbsent",
                                    std::bind(&AgribotBehaviorTreeNode::simpleCheck, this, ToCheck::obstacle));
  factory_->registerSimpleCondition(
      "MobileAppErrorAbsent", std::bind(&AgribotBehaviorTreeNode::simpleCheck, this, ToCheck::mobile_app_error_flag));
}

std::string AgribotBehaviorTreeNode::getTreeLocation() const
{
  return ros::package::getPath("agribot_commander") + "/behavior_tree/agribot_behavior.xml";
}

void AgribotBehaviorTreeNode::initTreeNodesWithRosDependencies()
{
  for (auto& node : tree_->nodes)
  {
    if (auto switch_control_arbiter = dynamic_cast<SwitchControlArbiters*>(node.get());
        switch_control_arbiter != nullptr)
    {
      switch_control_arbiter->initToolArbiterClient(tool_arbiter_client_);
      switch_control_arbiter->initCmdVelArbiterClients(cmd_vel_manual_client_, cmd_vel_navigation_client_,
                                                       cmd_vel_stop_client_);
    }
    if (auto check_start_conditions = dynamic_cast<CheckStartConditions*>(node.get());
        check_start_conditions != nullptr)
    {
      check_start_conditions->initSharedFields(task_name_, robot_mode_);
    }
    if (auto handle = dynamic_cast<Handle*>(node.get()); handle != nullptr)
    {
      handle->initSharedFields(task_name_, move_base_monitor_result_);
      handle->initPublishers(waypoints_publisher_, move_base_command_publisher_);
    }
  }
}

void AgribotBehaviorTreeNode::initSharedData()
{
  task_name_ = std::make_shared<Shared<std::string>>();
  robot_mode_ = std::make_shared<Shared<industrial_msgs::RobotMode>>();
  move_base_monitor_result_ = std::make_shared<Shared<agribot_msgs::AgribotMoveBaseMonitorResult>>();
}

void AgribotBehaviorTreeNode::initAtomics()
{
  obstacle_absent_.store(true);
  error_absent_.store(true);
}

// TODO(someone): while refactoring change state_machine -> behavior_tree in topic names
AgribotBehaviorTreeNode::AgribotBehaviorTreeNode(int argc, char** argv)
  : factory_{ std::make_unique<BT::BehaviorTreeFactory>() }
{
  // ROS init
  ros::init(argc, argv, "agribot_tools_control_node");
  node_handle_ = std::make_shared<ros::NodeHandle>();

  // Initialize variables shared between tree nodes
  initSharedData();
  // Initialize atomic variables used to check conditions
  initAtomics();

  // Publishers
  move_base_command_publisher_ = std::make_shared<ros::Publisher>(
      node_handle_->advertise<agribot_msgs::AgribotMoveBaseCommand>("/agribot/move_base_monitor/command", 1));
  waypoints_publisher_ = std::make_shared<ros::Publisher>(
      node_handle_->advertise<agribot_msgs::NavSatFixOrientation>("/agribot/waypoints", 1000));

  // Subscribers
  task_name_subscriber_ =
      node_handle_->subscribe("/agribot/state_machine/task", 1, &AgribotBehaviorTreeNode::taskNameCallback, this);
  robot_mode_subscriber_ =
      node_handle_->subscribe("/agribot/state_machine/mode", 1, &AgribotBehaviorTreeNode::robotModeCallback, this);
  move_base_result_subscriber_ = node_handle_->subscribe("/agribot/move_base_monitor/result", 1,
                                                         &AgribotBehaviorTreeNode::moveBaseMonitorResultCallback, this);
  obstacle_detected_subscriber_ = node_handle_->subscribe("/agribot/obstacle_detected", 1,
                                                          &AgribotBehaviorTreeNode::obstacleDetectedCallback, this);
  mobile_app_error_flag_subscriber_ = node_handle_->subscribe(
      "/agribot/state_machine/error", 1, &AgribotBehaviorTreeNode::mobileAppErrorFlagCallback, this);
  // Parameters

  // Services
  tool_arbiter_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<agribot_msgs::ToolManagerControlSource>("agribot/tool_management/control_source"));
  cmd_vel_manual_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_teleop"));
  cmd_vel_navigation_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_navigation"));
  cmd_vel_stop_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_stop"));

  // Register all custom actions on nodes factory and load tree from XML file
  registerNodes();
  tree_ = std::make_unique<BT::Tree>(factory_->createTreeFromFile(getTreeLocation()));

  // Register ZeroMQ publisher for Groot tree view:
  tree_state_publisher_ = std::make_unique<BT::PublisherZMQ>(*tree_);

  initTreeNodesWithRosDependencies();
}

void AgribotBehaviorTreeNode::run()
{
  ros::Rate rate(10);
  ros::AsyncSpinner spinner(4);
  spinner.start();
  while (ros::ok())
  {
    tree_->tickRoot();
    ros::spinOnce();
    rate.sleep();
  }
}

void AgribotBehaviorTreeNode::taskNameCallback(const std_msgs::String::ConstPtr& msg)
{
  task_name_->writeData(msg->data);
}

void AgribotBehaviorTreeNode::robotModeCallback(const industrial_msgs::RobotMode::ConstPtr& msg)
{
  robot_mode_->writeData(*msg);
}

void AgribotBehaviorTreeNode::moveBaseMonitorResultCallback(
    const agribot_msgs::AgribotMoveBaseMonitorResult::ConstPtr& msg)
{
  move_base_monitor_result_->writeData(*msg);
}

void AgribotBehaviorTreeNode::obstacleDetectedCallback(const agribot_msgs::AgribotObstacleDetection::ConstPtr& msg)
{
  if (msg->status == msg->OBSTACLE_DETECTED)
  {
    obstacle_absent_.store(false);
    return;
  }
  obstacle_absent_.store(true);
}

void AgribotBehaviorTreeNode::mobileAppErrorFlagCallback(const agribot_msgs::AgribotAppErrorFlag::ConstPtr& msg)
{
  if (msg->error_flag == msg->APP_PROTOCOL_ERROR || msg->error_flag == msg->APP_MANUAL_ERROR)
  {
    error_absent_.store(false);
    return;
  }
  error_absent_.store(true);
}

}  // namespace agribot_behavior
