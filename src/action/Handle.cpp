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
        ROS_WARN_STREAM("[BehaviorTree][Handle] Running manual action");
        handleManualControl();
        break;
      case tool_manager::ControlSource::Navigation:
        ROS_WARN_STREAM("[BehaviorTree][Handle] Running autonomous action");
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
    std::shared_ptr<Shared<std::string>> task_name,
    std::shared_ptr<Shared<agribot_msgs::AgribotMoveBaseMonitorResult>> move_base_monitor_result)
{
  task_name_ = task_name;
  move_base_monitor_result_ = move_base_monitor_result;
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
    ROS_WARN_STREAM("[BehaviorTree][Handle] Loaded path " << path_feeder_.getLoadedPathLocation());
    ros::Rate rate(10);
    while (!halt_requested_ && ros::ok() && !path_feeder_.feedingCompleted())
    {
      publishWaypoint(path_feeder_.getPathPoint());
      ros::spinOnce();
      rate.sleep();
    }
    if (moveBaseReady())
    {
      ROS_WARN_STREAM("[BehaviorTree][Handle] Move base is active. Started executing task.");
      while (!halt_requested_ && ros::ok() && moveBaseActive())
      {
        ros::spinOnce();
        rate.sleep();
      }
      ROS_WARN_STREAM("[BehaviorTree][Handle] Autonomous task finished with move base status "
                      << static_cast<int>(move_base_monitor_result_->getData().result));
    }
    else
    {
      ROS_ERROR_STREAM("[BehaviorTree][Handle] Move base activate timeout");
    }
    resetAutonomousDriving();
  }
  else
  {
    ROS_ERROR_STREAM("[BehaviorTree][Handle] Failed to load path from task " << task_name_->getData());
  }
}

void Handle::publishWaypoint(const agribot_msgs::NavSatFixOrientation& waypoint) const
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

bool Handle::moveBaseActive()
{
  if (move_base_monitor_result_->getData().result != agribot_msgs::AgribotMoveBaseMonitorResult::ACTIVE)
  {
    return false;
  }
  return true;
}

bool Handle::moveBaseReady()
{
  std::size_t timeout = 50;  // 5s
  std::size_t cnt = 0;
  ros::Rate rate(10);
  while (!moveBaseActive())
  {
    if (cnt >= timeout)
    {
      return false;
    }
    ros::spinOnce();
    rate.sleep();
    ++cnt;
  }
  return true;
}

}  // namespace agribot_behavior
