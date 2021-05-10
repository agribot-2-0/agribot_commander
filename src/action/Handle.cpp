/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/Handle.h>

namespace agribot_behavior
{
Handle::Handle(const std::string& name, const BT::NodeConfiguration& config) : AsyncActionNode(name, config)
{
  halt_requested_.store(false);
}

BT::PortsList Handle::providedPorts()
{
  return { BT::InputPort<tool_manager::ControlSource>("action_to_handle") };
}

BT::NodeStatus Handle::tick()
{
  BT::Optional<tool_manager::ControlSource> msg = TreeNode::getInput<tool_manager::ControlSource>("action_to_handle");
  if (!msg)
  {
    ROS_ERROR_STREAM("[BehaviorTree][Handle] Missing required input [action_to_handle]: " << msg.error());
    return BT::NodeStatus::FAILURE;
  }
  else
  {
    switch (msg.value())
    {
      case tool_manager::ControlSource::Manual:
        ROS_WARN_STREAM("[BehaviorTree][HandleManual] Running manual action");
        handleManualControl();
        break;
      case tool_manager::ControlSource::Navigation:
        ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Running autonomous action");
        handleAutonomousControl();
        break;
      default:
        break;
    }
    halt_requested_.store(false);
    return BT::NodeStatus::SUCCESS;
  }
}

void Handle::initSharedFields(
    std::shared_ptr<SharedField<std::string>> task_name,
    std::shared_ptr<SharedField<agribot_msgs::AgribotNavigationManagerResult>> navigation_manager_result)
{
  task_name_ = task_name;
  navigation_manager_result_ = navigation_manager_result;
}

void Handle::initPublishers(std::shared_ptr<ros::Publisher> waypoints_publisher,
                            std::shared_ptr<ros::Publisher> move_base_command_publisher)
{
  waypoints_publisher_ = waypoints_publisher;
  move_base_command_publisher_ = move_base_command_publisher;
}

void Handle::halt()
{
  halt_requested_.store(true);
}

void Handle::initPathFeeder(const std::string& path_folder, const std::string& task_folder)
{
  path_feeder_.init(path_folder, task_folder);
}

// Manual control is idle action since another ROS nodes are handling it
void Handle::handleManualControl()
{
  ros::Rate rate(10);
  while (!halt_requested_ && ros::ok())
  {
    ros::spinOnce();
    rate.sleep();
  }
}

void Handle::handleAutonomousControl()
{
  if (path_feeder_.loadPath(task_name_->getData()))
  {
    ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Loaded path " << path_feeder_.getLoadedPathLocation());
    ros::Rate rate(10);
    while (!halt_requested_ && ros::ok() && !path_feeder_.feedingCompleted())
    {
      publishWaypoint(path_feeder_.getPathPoint());
      ros::spinOnce();
      rate.sleep();
    }
    if (moveBaseReady())
    {
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Move base is active. Started executing task");
      while (!halt_requested_ && ros::ok() && moveBaseActive())
      {
        ros::spinOnce();
        rate.sleep();
      }
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Autonomous task finished with move base status "
                      << static_cast<int>(navigation_manager_result_->getData().result));
    }
    else
    {
      ROS_ERROR_STREAM("[BehaviorTree][HandleAuto] Move base activate timeout");
    }
    resetAutonomousDriving();
  }
  else
  {
    ROS_ERROR_STREAM("[BehaviorTree][HandleAuto] Failed to load path from task " << task_name_->getData());
  }
}

void Handle::publishWaypoint(const sensor_msgs::NavSatFix& waypoint) const
{
  waypoints_publisher_->publish(waypoint);
}

void Handle::publishMoveBaseCommand(const uint8_t command) const
{
  move_base_command_publisher_->publish([command] {
    agribot_msgs::AgribotMoveBaseCommand msg;
    msg.command = command;
    return msg;
  }());
}

void Handle::resetAutonomousDriving()
{
  publishMoveBaseCommand(agribot_msgs::AgribotMoveBaseCommand::ABORT);
  path_feeder_.reset();
}

bool Handle::moveBaseActive() const
{
  return navigation_manager_result_->getData().result == agribot_msgs::AgribotNavigationManagerResult::ACTIVE;
}

bool Handle::moveBaseReady() const
{
  ros::Duration timeout{5};
  ros::Rate rate{10};
  ros::Time start_time = ros::Time::now();
  while (!moveBaseActive())
  {
    ros::Time time_now = ros::Time::now();
    if (time_now - start_time >= timeout)
    {
      return false;
    }
    ros::spinOnce();
    rate.sleep();
  }
  return true;
}

}  // namespace agribot_behavior
