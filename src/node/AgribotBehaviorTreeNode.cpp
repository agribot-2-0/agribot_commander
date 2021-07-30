/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/node/AgribotBehaviorTreeNode.h>

namespace agribot_behavior
{
typedef ros::console::Level RosLogLevel;

BT::NodeStatus AgribotBehaviorTreeNode::simpleConditionCheck(const CheckAbsenceOf check)
{
  switch (check)
  {
    case CheckAbsenceOf::MobileAppErrorFlag:
      if (error_absent_.load())
      {
        log_once_.reset("mobile_app_error");
        return BT::NodeStatus::SUCCESS;
      }
      log_once_.log("mobile_app_error", RosLogLevel::Warn, "[AgribotBehavior] MobileAppError detected");
      break;
    case CheckAbsenceOf::Obstacle:
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

void AgribotBehaviorTreeNode::registerTreeNodes()
{
  factory_->registerNodeType<CheckStartConditions>("CheckStartConditions");
  factory_->registerNodeType<SwitchControlArbiters>("SwitchControlArbiters");
  factory_->registerNodeType<Handle>("Handle");
  factory_->registerNodeType<LightsControl>("LightsControl");
  factory_->registerNodeType<SwitchTtcontrolState>("SwitchTtcontrolState");
  factory_->registerNodeType<ControlEngine>("ControlEngine");
  factory_->registerSimpleCondition(
      "ObstacleAbsent", std::bind(&AgribotBehaviorTreeNode::simpleConditionCheck, this, CheckAbsenceOf::Obstacle));
  factory_->registerSimpleCondition("MobileAppErrorAbsent", std::bind(&AgribotBehaviorTreeNode::simpleConditionCheck,
                                                                      this, CheckAbsenceOf::MobileAppErrorFlag));
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
      check_start_conditions->initSharedFields(current_task_name_, robot_mode_, ttcontrol_state_);
    }
    if (auto handle = dynamic_cast<Handle*>(node.get()); handle != nullptr)
    {
      handle->initSharedFields(current_task_name_, previous_task_name_, path_progression_,
                               autonomous_path_action_feedback_, robot_mode_, tpl_front_config_publisher_,
                               tpl_back_config_publisher_);
      handle->initActionlibs(autonomous_path_action_client_);
      handle->initPathParser(path_folder_, task_folder_);
      handle->initTplHandler(path_folder_, task_folder_);
    }
    if (auto lights_control = dynamic_cast<LightsControl*>(node.get()); lights_control != nullptr)
    {
      lights_control->initLightsClient(hal_lights_client_);
    }
    if (auto switch_ttcontrol_state = dynamic_cast<SwitchTtcontrolState*>(node.get());
        switch_ttcontrol_state != nullptr)
    {
      switch_ttcontrol_state->initTtcontrolStateClients(ttcontrol_safe_state_client_, ttcontrol_auto_state_client_);
    }
    if (auto control_engine = dynamic_cast<ControlEngine*>(node.get()); control_engine != nullptr)
    {
      control_engine->initEngineControlClients(engine_start_client_, engine_stop_client_);
    }
  }
}

void AgribotBehaviorTreeNode::initSharedData()
{
  current_task_name_ = std::make_shared<SharedField<std::string>>();
  previous_task_name_ = std::make_shared<SharedField<std::string>>();
  path_progression_ = std::make_shared<SharedField<std::size_t>>();
  robot_mode_ = std::make_shared<SharedField<industrial_msgs::RobotMode>>();
  autonomous_path_action_feedback_ = std::make_shared<SharedField<agribot_msgs::AgribotNavigationManagerResult>>();
  ttcontrol_state_ = std::make_shared<SharedField<agribot_msgs::AgribotTtcontrolState>>();
}

void AgribotBehaviorTreeNode::initAtomics()
{
  obstacle_absent_.store(true);
  error_absent_.store(true);
}

void AgribotBehaviorTreeNode::initRosActionlibClients()
{
  autonomous_path_action_client_ =
      std::make_shared<actionlib::SimpleActionClient<agribot_msgs::PerformAutonomousPathAction>>(
          "/agribot/navigation_manager/path_action", true);
}

void AgribotBehaviorTreeNode::initRosActionlibServers()
{
  autonomous_task_action_server_ =
      std::make_unique<actionlib::SimpleActionServer<agribot_msgs::PerformAutonomousTaskAction>>(
          (*node_handle_), "/agribot/behavior_tree/task_action",
          boost::bind(&AgribotBehaviorTreeNode::performAutonomousTaskCallback, this, _1), false);

  autonomous_task_action_server_->start();
}

void AgribotBehaviorTreeNode::initRosSubscribers()
{
  obstacle_detected_subscriber_ =
      node_handle_->subscribe("obstacle_detected", 1, &AgribotBehaviorTreeNode::obstacleDetectedRosCallback, this);
  mobile_app_error_flag_subscriber_ =
      node_handle_->subscribe("mobile_app_error", 1, &AgribotBehaviorTreeNode::mobileAppErrorFlagRosCallback, this);
  ttcontrol_state_subscriber_ =
      node_handle_->subscribe("/agribot/ttcontrol_state", 1, &AgribotBehaviorTreeNode::ttcontrolStateCallback, this);
}

void AgribotBehaviorTreeNode::initRosPublishers()
{
  robot_mode_publisher_ = std::make_shared<ros::Publisher>(
      node_handle_->advertise<industrial_msgs::RobotMode>("/agribot/behavior_tree/robot_mode", 1));
  tpl_front_config_publisher_ = std::make_shared<ros::Publisher>(
      node_handle_->advertise<agribot_msgs::AgribotTool>("/agribot/control/tool/front/navigation", 1));
  tpl_back_config_publisher_ = std::make_shared<ros::Publisher>(
      node_handle_->advertise<agribot_msgs::AgribotTool>("/agribot/control/tool/back/navigation", 1));
}

void AgribotBehaviorTreeNode::initRosServices()
{
  // Services
  tool_arbiter_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<agribot_msgs::ToolManagerControlSource>("/agribot/tool_management/control_source"));
  cmd_vel_manual_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_teleop"));
  cmd_vel_navigation_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_navigation"));
  cmd_vel_stop_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Empty>("/cmd_vel_arbiter_node/switch_to_stop"));
  hal_lights_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<agribot_msgs::LampsControl>("/agribot/hal_lights/change_lamps_configuration"));
  ttcontrol_safe_state_client_ =
      std::make_shared<ros::ServiceClient>(node_handle_->serviceClient<std_srvs::Trigger>("/agribot/set_safe_state"));
  ttcontrol_auto_state_client_ = std::make_shared<ros::ServiceClient>(
      node_handle_->serviceClient<std_srvs::Trigger>("/agribot/set_autonomous_mode"));
  engine_start_client_ =
      std::make_shared<ros::ServiceClient>(node_handle_->serviceClient<std_srvs::Trigger>("/agribot/engine_start"));
  engine_stop_client_ =
      std::make_shared<ros::ServiceClient>(node_handle_->serviceClient<std_srvs::Trigger>("/agribot/engine_stop"));
}

AgribotBehaviorTreeNode::AgribotBehaviorTreeNode(int argc, char** argv)
  : factory_{ std::make_unique<BT::BehaviorTreeFactory>() }
{
  // ROS init
  ros::init(argc, argv, "agribot_tools_control_node");
  std::experimental::filesystem::path agribot_home_location = std::getenv("AGRIBOT_HOME");
  node_handle_ = std::make_shared<ros::NodeHandle>("~");
  try
  {
    task_folder_ = agribot_home_location / ros_common_functions::getRosParam<std::string>("agribot_map/tasks_path");
    path_folder_ =
        agribot_home_location / ros_common_functions::getRosParam<std::string>("agribot_map/paths_path") / "paths";
  }
  catch (const agribot_exceptions::ParameterNotSet& ex)
  {
    ROS_ERROR_STREAM("[BehaviorTree] " << ex.what());
    ros::shutdown();
  }

  // Initialize variables shared between tree nodes
  initSharedData();
  // Initialize atomic variables used to check conditions
  initAtomics();

  // Register all custom actions on nodes factory and load tree from XML file
  registerTreeNodes();
  tree_ = std::make_unique<BT::Tree>(factory_->createTreeFromFile(getTreeLocation()));

  // Register ZeroMQ publisher for Groot tree view:
  tree_state_publisher_ = std::make_unique<BT::PublisherZMQ>(*tree_);

  initRosActionlibClients();
  initRosActionlibServers();
  initRosSubscribers();
  initRosServices();
  initRosPublishers();
  initTreeNodesWithRosDependencies();
}

void AgribotBehaviorTreeNode::run()
{
  ros::Rate rate(50);
  ros::AsyncSpinner spinner(4);
  spinner.start();
  while (ros::ok())
  {
    robot_mode_publisher_->publish(robot_mode_->getData());
    tree_->tickRoot();
    ros::spinOnce();
    rate.sleep();
  }
}

void AgribotBehaviorTreeNode::obstacleDetectedRosCallback(const agribot_msgs::ObstacleDetectionStatus::ConstPtr& msg)
{
  if (msg->status == msg->OBSTACLE_DETECTED)
  {
    obstacle_absent_.store(false);
    return;
  }
  // TODO(all) - handle WAITING_FOR_PATH status
  // else if (msg->status == msg->WAITING_FOR_PATH)
  // {
  //   obstacle_absent_.store(false);
  //   return;
  // }
  else if (msg->status == msg->WAITING_FOR_OBJECTS)
  {
    obstacle_absent_.store(false);
    return;
  }
  else if (msg->status == msg->UNKNOWN)
  {
    obstacle_absent_.store(false);
    return;
  }

  obstacle_absent_.store(true);
}

void AgribotBehaviorTreeNode::mobileAppErrorFlagRosCallback(const agribot_msgs::AgribotAppErrorFlag::ConstPtr& msg)
{
  if (msg->error_flag == msg->APP_PROTOCOL_ERROR || msg->error_flag == msg->APP_MANUAL_ERROR)
  {
    error_absent_.store(false);
    return;
  }
  error_absent_.store(true);
}

void AgribotBehaviorTreeNode::ttcontrolStateCallback(const agribot_msgs::AgribotTtcontrolState::ConstPtr& msg)
{
  ttcontrol_state_->writeData(*msg);
}

void AgribotBehaviorTreeNode::performAutonomousTaskCallback(const agribot_msgs::PerformAutonomousTaskGoalConstPtr& goal)
{
  current_task_name_->writeData(goal->task_name);
  if (goal->task_name != "")
  {
    robot_mode_->writeData([] {
      industrial_msgs::RobotMode msg;
      msg.val = industrial_msgs::RobotMode::AUTO;
      return msg;
    }());
    ros::Rate loop_rate(10);
    while (ros::ok() && !autonomous_task_action_server_->isPreemptRequested() && current_task_name_->getData() != "")
    {
      // Publish feedback to TaskSupervisor
      autonomous_task_action_server_->publishFeedback([this] {
        agribot_msgs::PerformAutonomousTaskFeedback feedback;
        feedback.feedback = autonomous_path_action_feedback_->getData();
        return feedback;
      }());
      if (pathFinished())
      {
        current_task_name_->writeData("");
        autonomous_path_action_feedback_->writeData(agribot_msgs::AgribotNavigationManagerResult());
        autonomous_task_action_server_->setSucceeded([this] {
          agribot_msgs::PerformAutonomousTaskResult path_action_result;
          path_action_result.finished = true;
          return path_action_result;
        }());
        robot_mode_->writeData([] {
          industrial_msgs::RobotMode msg;
          msg.val = industrial_msgs::RobotMode::UNKNOWN;
          return msg;
        }());
        return;
      }
      if (pathAborted())
      {
        ROS_ERROR_STREAM("[BehaviorTree] Task aborted. Move base error");
        break;
      }
      if (ttcontrolInManualMode())
      {
        break;
      }
      ros::spinOnce();
      loop_rate.sleep();
    }
  }
  autonomous_path_action_feedback_->writeData(agribot_msgs::AgribotNavigationManagerResult());
  autonomous_task_action_server_->setSucceeded([this] {
    agribot_msgs::PerformAutonomousTaskResult path_action_result;
    path_action_result.finished = false;
    return path_action_result;
  }());
  robot_mode_->writeData([] {
    industrial_msgs::RobotMode msg;
    msg.val = industrial_msgs::RobotMode::UNKNOWN;
    return msg;
  }());
}  // namespace agribot_behavior

bool AgribotBehaviorTreeNode::ttcontrolInManualMode()
{
  return ttcontrol_state_->getData().manual_state == agribot_msgs::AgribotTtcontrolState::STEERING_ACTIVE;
}

bool AgribotBehaviorTreeNode::pathFinished()
{
  return (autonomous_path_action_feedback_->getData().result == autonomous_path_action_feedback_->getData().COMPLETED);
}

bool AgribotBehaviorTreeNode::pathAborted()
{
  return (autonomous_path_action_feedback_->getData().result == autonomous_path_action_feedback_->getData().ABORTED);
}

}  // namespace agribot_behavior
