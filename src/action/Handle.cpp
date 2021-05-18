/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <agribot_behavior/action/Handle.h>
#include <agribot_common/CommonFunctions/navigation_manager_result.h>

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
    return BT::NodeStatus::FAILURE;
  }
}

void Handle::initSharedFields(
    std::shared_ptr<SharedField<std::string>> current_task_name,
    std::shared_ptr<SharedField<std::string>> previous_task_name,
    std::shared_ptr<SharedField<std::size_t>> path_progression,
    std::shared_ptr<SharedField<agribot_msgs::AgribotNavigationManagerResult>> autonomous_path_action_feedback)
{
  current_task_name_ = current_task_name;
  previous_task_name_ = previous_task_name;
  path_progression_ = path_progression;
  autonomous_path_action_feedback_ = autonomous_path_action_feedback;
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
  path_parser_->setPathName(current_task_name_->getData());
  std::vector<sensor_msgs::NavSatFix> path_points = path_parser_->parsePath();
  if (!path_points.empty())
  {
    if (taskUnchanged())  // continue path from last visited waypoint
    {
      ROS_WARN_STREAM("[BehaviorTree][HandleAuto] Path unchanged. Continuing path from waypoint "
                      << static_cast<int>(path_progression_->getData()));
    }
    else
    {
      path_progression_->writeData(0);
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
      ROS_WARN_STREAM(
          "[BehaviorTree][HandleAuto] Autonomous task finished with NavigationManager status "
          << common_functions::navigationManagerResultToString(autonomous_path_action_feedback_->getData()));
    }
    else
    {
      ROS_ERROR_STREAM("[BehaviorTree][HandleAuto] Move base activate timeout");
    }
    resetAutonomousDriving();
  }
  else
  {
    ROS_ERROR_STREAM("[BehaviorTree][HandleAuto] Failed to load path from task " << current_task_name_->getData());
  }
}

void Handle::resetAutonomousDriving()
{
  if (pathFinished())
  {
    path_progression_->writeData(0);
    current_task_name_->writeData("");
  }
  else
  {
    path_progression_->writeData(autonomous_path_action_feedback_->getData().current_waypoint);
    ROS_ERROR_STREAM("[BehaviorTree][HandleAuto] Saved path progression "
                     << path_progression_->getData() << " out of "
                     << autonomous_path_action_feedback_->getData().total_waypoints << " waypoints");
    previous_task_name_->writeData(current_task_name_->getData());
    autonomous_path_action_client_->cancelGoal();
    autonomous_path_action_feedback_->writeData([] {
      agribot_msgs::AgribotNavigationManagerResult feedback;
      feedback.result = feedback.PAUSED;
      return feedback;
    }());
  }
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
      [this, path_points]() {
        agribot_msgs::PerformAutonomousPathGoal goal_path;
        goal_path.path = path_points;
        goal_path.path_progression = path_progression_->getData();
        return goal_path;
      }(),
      boost::bind(&agribot_behavior::Handle::autonomousPathDoneCallback, this, _1, _2),
      boost::bind(&agribot_behavior::Handle::autonomousPathActiveCallback, this),
      boost::bind(&agribot_behavior::Handle::autonomousPathFeedbackCallback, this, _1));
}

void Handle::autonomousPathDoneCallback(const actionlib::SimpleClientGoalState& state,
                                        const agribot_msgs::PerformAutonomousPathResultConstPtr& result)
{
  static_cast<void>(state);
  static_cast<void>(result);
}

void Handle::autonomousPathActiveCallback()
{
}

void Handle::autonomousPathFeedbackCallback(const agribot_msgs::PerformAutonomousPathFeedbackConstPtr& feedback)
{
  autonomous_path_action_feedback_->writeData(feedback->feedback);
}

bool Handle::taskUnchanged()
{
  return current_task_name_->getData() == previous_task_name_->getData();
}

bool Handle::pathFinished()
{
  return (autonomous_path_action_feedback_->getData().result == autonomous_path_action_feedback_->getData().COMPLETED);
}

}  // namespace agribot_behavior
