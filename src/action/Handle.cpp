/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/Handle.h>

namespace agribot_behavior
{
Handle::Handle(const std::string& name, const BT::NodeConfiguration& config) : AsyncActionNode(name, config)
{
  halt_requested_.store(false);
  previous_task_name_->writeData("");
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
    return BT::NodeStatus::FAILURE;
  }
}

void Handle::initSharedFields(std::shared_ptr<SharedField<std::string>> current_task_name,
                              std::shared_ptr<SharedField<std::string>> previous_task_name,
                              std::shared_ptr<SharedField<std::size_t>> current_waypoint)
{
  current_task_name_ = current_task_name;
  previous_task_name_ = previous_task_name;
  current_waypoint_ = current_waypoint;
}

void Handle::initActionlibs(std::shared_ptr<actionlib::SimpleActionClient<agribot_msgs::PerformAutonomousPathAction>>
                                autonomous_path_action_client)
{
  autonomous_path_action_client_ = autonomous_path_action_client;
}

void Handle::halt()
{
  halt_requested_.store(true);
}

void Handle::initPathParser(const std::string& path_folder, const std::string& task_folder)
{
  path_parser_ = std::make_unique<PathParser>(path_folder, task_folder);
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
  path_parser_->setPathName(task_name_->getData());
  std::vector<sensor_msgs::NavSatFix> path_points = path_parser_->parsePath();
  if (!path_points.empty())
  {
    ROS_WARN_STREAM("---------------previous_task:  " << previous_task_name_->getData());
    ROS_WARN_STREAM("---------------current_task: " << task_name_->getData());
    if (taskUnchanged())  // continue path from last visited waypoint
    {
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Continuing path from waypoint "
                      << static_cast<int>(current_waypoint_->getData()));
      removeVisitedWaypointsFromPath(path_points);
    }
    sendAutonomousPathGoal(path_points);
    ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Loaded path " << path_parser_->getPathLocation());
    if (navigationManagerReady())
    {
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Move base is active. Started executing task");
      ros::Rate rate{ 10 };
      while (!halt_requested_ && ros::ok() && navigationManagerActive())
      {
        ros::spinOnce();
        rate.sleep();
      }
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Autonomous task finished with NavigationManager status "
                      << static_cast<int>(autonomous_path_action_feedback_.result));
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

void Handle::resetAutonomousDriving()
{
  current_waypoint_->writeData(autonomous_path_action_feedback_.current_waypoint);
  previous_task_name_->writeData(task_name_->getData());
  task_name_->writeData("");  // TODO(mwiadoms): is it ok?
  autonomous_path_action_client_->cancelAllGoals();
}

bool Handle::navigationManagerActive() const
{
  return autonomous_path_action_client_->getState().state_ == autonomous_path_action_client_->getState().ACTIVE;
}

bool Handle::navigationManagerReady() const
{
  ros::Duration timeout{ 5 };
  ros::Rate rate{ 10 };
  ros::Time start_time = ros::Time::now();
  while (!navigationManagerActive())
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

void Handle::sendAutonomousPathGoal(const std::vector<sensor_msgs::NavSatFix>& path_points)
{
  autonomous_path_action_client_->sendGoal(
      [path_points]() {
        agribot_msgs::PerformAutonomousPathGoal goal_path;
        goal_path.path = path_points;
        return goal_path;
      }(),
      boost::bind(&agribot_behavior::Handle::autonomousPathDoneCallback, this, _1, _2),
      boost::bind(&agribot_behavior::Handle::autonomousPathActiveCallback, this),
      boost::bind(&agribot_behavior::Handle::autonomousPathFeedbackCallback, this, _1));
}

// Called once when the goal completes
void Handle::autonomousPathDoneCallback(const actionlib::SimpleClientGoalState& state,
                                        const agribot_msgs::PerformAutonomousPathResultConstPtr& result)
{
  ROS_WARN_STREAM("[BehaviorTree][autonomousPathDoneCallback] finish status: " << static_cast<int>(result->finished));
  ROS_WARN_STREAM("[BehaviorTree][autonomousPathDoneCallback] state: " << static_cast<int>(state.state_));

  resetAutonomousDriving();
}

// Called once when the goal becomes active
void Handle::autonomousPathActiveCallback()
{
}

// Called every time feedback is received for the goal
void Handle::autonomousPathFeedbackCallback(const agribot_msgs::PerformAutonomousPathFeedbackConstPtr& feedback)
{
  ROS_ERROR_STREAM("[ACTIONLIB] Current waypoint: " << static_cast<int>(feedback->feedback.current_waypoint));
  autonomous_path_action_feedback_ = feedback->feedback;
}

bool Handle::taskUnchanged()
{
  return task_name_->getData() == previous_task_name_->getData();
}

void Handle::removeVisitedWaypointsFromPath(std::vector<sensor_msgs::NavSatFix>& path_points)
{
  path_points.erase(path_points.begin(), path_points.begin() + current_waypoint_->getData());
}

}  // namespace agribot_behavior
