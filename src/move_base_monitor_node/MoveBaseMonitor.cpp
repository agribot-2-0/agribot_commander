/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <agribot_commander/MoveBaseMonitor.h>
#include <visualization_msgs/MarkerArray.h>

MoveBaseMonitor::MoveBaseMonitor()
{
  _waypointsSub = _nh.subscribe("/agribot/path_point",
                                1000,
                                &MoveBaseMonitor::waypointCallback,
                                this);
  _stateMachineSub = _nh.subscribe("/agribot/move_base_monitor/command",
                                   10,
                                   &MoveBaseMonitor::stateMachineCallback,
                                   this);
  _stateMachinePub = _nh.advertise<agribot_msgs::AgribotMoveBaseMonitorResult>("/agribot/move_base_monitor/result", 10);

  waypoint_marker_pub_ = _nh.advertise<visualization_msgs::MarkerArray>("/agribot/move_base_monitor/waypoints", 1);
  plan_pub_ = _nh.advertise<nav_msgs::Path>("/agribot/move_base_monitor/path", 1);
  // tell the action client that we want to spin a thread by default
  ac = new MoveBaseClient("move_base", true);
  // wait for the action server to come up
  while (!ac->waitForServer(ros::Duration(5.0)))
  {
    ROS_INFO_STREAM("[MoveBaseMonitor]Waiting for the move_base"
                    << "action server to come up");
  }

  resultMsg.result = resultMsg.NOACTION;
  resultMsg.current_waypoint = 0;
  resultMsg.total_waypoints = 0;
  _pauseGoal = false;
  _pauseGoalLatch = false;
  _pathLoaded = false;
  _userAbort = false;
}

void MoveBaseMonitor::stateMachineCallback(const agribot_msgs::AgribotMoveBaseCommand &command_msg)
{
  // if the path is blocked then cancel the current goal and set the pause flag
  if (command_msg.command == command_msg.PAUSE)
  {
    ac->cancelAllGoals();
    _pauseGoal = true;
    _pauseGoalLatch = true;
  }
  else if (command_msg.command == command_msg.CONTINUE)
  {
    _pauseGoal = false;
  }
  // if we want to abort the current task by user request
  else if (command_msg.command == command_msg.ABORT)
  {
    _userAbort = true;
  }
}

void MoveBaseMonitor::isPaused()
{
  bool cancelTrig = true;
  while (_pauseGoal)
  {
    if (cancelTrig)
    {
      ac->cancelAllGoals();
      ROS_INFO_STREAM("[MoveBaseMonitor] Goal PAUSED");
      cancelTrig = false;
    }
    resultMsg.result = resultMsg.PAUSED;
    _stateMachinePub.publish(resultMsg);
    ros::Duration(0.1).sleep(); // sleep for half a second
    ros::spinOnce();
  }
}

void MoveBaseMonitor::waypointCallback(
    const geometry_msgs::PoseStampedConstPtr &waypoint)
{
  if (clear_waypoints_)
  {
    waypoints_.clear();
    clear_waypoints_ = false;
  }

  // add waypoint to the waypoint vector
  waypoints_.push_back(geometry_msgs::PoseStamped());
  waypoints_.back().header = waypoint->header;
  waypoints_.back().pose = waypoint->pose;

  if (waypoints_.size() < 2)
    return;

  geometry_msgs::Pose *p1 = &(waypoints_.end() - 2)->pose;
  geometry_msgs::Pose *p2 = &(waypoints_.end() - 1)->pose;

  // calculate orientation of waypoints
  double yaw = atan2(p2->position.y - p1->position.y,
                     p2->position.x - p1->position.x);

  // calculate distance between latest two waypoints
  // and check if it surpasses the threshold epsilon
  double dist = hypot(p1->position.x - p2->position.x, p1->position.y - p2->position.y);

  if (dist < 0.01)
  {
    p2->orientation = p1->orientation;
    path_.header = waypoint->header;
    path_.poses.clear();
    path_.poses.insert(path_.poses.end(), waypoints_.begin(), waypoints_.end());
    createAndPublishMarkersFromPath(path_.poses);
    plan_pub_.publish(path_);
    _pathLoaded = true;
    _userAbort = false;
  }
}

void MoveBaseMonitor::createAndPublishMarkersFromPath(const std::vector<geometry_msgs::PoseStamped> &path)
{
  // clear previous markers
  visualization_msgs::MarkerArray markers;
  visualization_msgs::Marker marker;
  marker.header = path[0].header;
  marker.ns = "/move_base_monitor/waypoints";
  marker.type = visualization_msgs::Marker::ARROW;
  marker.action = visualization_msgs::Marker::DELETEALL;
  marker.scale.x = 0.75;
  marker.scale.y = 0.1;
  marker.scale.z = 0.01;
  marker.color.a = 1.0;
  marker.color.r = 0.1;
  marker.color.g = 0.2;
  marker.color.b = 0.7;
  marker.id = 0;
  markers.markers.push_back(marker);
  waypoint_marker_pub_.publish(markers);
  marker.action = visualization_msgs::Marker::ADD;
  markers.markers.clear();

  for (size_t i = 0; i < path.size(); i++)
  {
    marker.id = i;
    marker.pose.position = path[i].pose.position;
    marker.pose.orientation = path[i].pose.orientation;
    markers.markers.push_back(marker);
  }

  waypoint_marker_pub_.publish(markers);
}

void MoveBaseMonitor::monitorPathWaypoints()
{
  move_base_msgs::MoveBaseGoal goal;
  actionlib::SimpleClientGoalState::StateEnum goal_result;
  ROS_INFO_STREAM("[MoveBaseMonitor] Start sending goals");
  int currentWaypoint = 0;
  bool load_point = true;
  auto pose = path_.poses[currentWaypoint];
  ros::Rate loop_rate(100);
  while (currentWaypoint < path_.poses.size())
  {
    resultMsg.current_waypoint = currentWaypoint;
    resultMsg.total_waypoints = path_.poses.size();
    if (_userAbort)
    {
      ac->cancelAllGoals();
      break;
    }
    // check if the current goal was pause, if so then wait
    isPaused();
    // check if the next point should be loaded
    if (load_point)
    {
      pose = path_.poses[currentWaypoint];
      ROS_INFO_STREAM("[MoveBaseMonitor] Point " << pose.pose.position.x << ", " << pose.pose.position.y << " Index "
                                                 << currentWaypoint);
      goal.target_pose = pose;
      ac->sendGoal(goal);
      load_point = false;
    }
    else
    {
      goal_result = ac->getState().state_;
      if (goal_result == actionlib::SimpleClientGoalState::ACTIVE ||
          goal_result == actionlib::SimpleClientGoalState::PENDING)
      {
        resultMsg.result = resultMsg.ACTIVE;
        _stateMachinePub.publish(resultMsg);
      }
      else if (goal_result == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        resultMsg.result = resultMsg.SUCCES;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] Goal succeeded");
        currentWaypoint++;
        load_point = true;
      }
      else if (goal_result == actionlib::SimpleClientGoalState::PREEMPTED)
      {
        // check if the current goal is paused or we should take next goal
        if (_pauseGoalLatch)
        {
          resultMsg.result = resultMsg.PAUSED;
          _stateMachinePub.publish(resultMsg);
          load_point = true;
          _pauseGoalLatch = false;
        }
        else
        {
          resultMsg.result = resultMsg.PREEMPTED;
          _stateMachinePub.publish(resultMsg);
          ROS_INFO_STREAM("[MoveBaseMonitor] Goal PREEMPTED");
          currentWaypoint++;
          load_point = true;
        }
      }
      else if (goal_result == actionlib::SimpleClientGoalState::ABORTED)
      {
        resultMsg.result = resultMsg.ABORTED;
        _stateMachinePub.publish(resultMsg);
        currentWaypoint++;
        load_point = true;
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was ABORTED!,"
                        << "trying next goal");
      }
      else if (goal_result == actionlib::SimpleClientGoalState::REJECTED)
      {
        resultMsg.result = resultMsg.FAILED;
        _stateMachinePub.publish(resultMsg);
        currentWaypoint++;
        load_point = true;
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was REJECTED!,"
                        << "trying next goal");
      }
      else if (goal_result == actionlib::SimpleClientGoalState::RECALLED)
      {
        resultMsg.result = resultMsg.FAILED;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was RECALLED!,"
                        << "trying next goal");
        currentWaypoint++;
        load_point = true;
      }
      else if (goal_result == actionlib::SimpleClientGoalState::LOST)
      {
        resultMsg.result = resultMsg.FAILED;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was LOST!,"
                        << "trying next goal");
        currentWaypoint++;
        load_point = true;
      }
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
  clear_waypoints_ = true;
  _pathLoaded = false;
  if (_userAbort == true)
  {
    _userAbort = false;
    ROS_INFO("[MoveBaseMonitor] Path aborted by user");
  }
  else
  {
    resultMsg.result = resultMsg.PATH_COMPLETED;
    _stateMachinePub.publish(resultMsg);
    ROS_INFO("[MoveBaseMonitor] Path ended");
  }
}

void MoveBaseMonitor::run()
{
  ros::Rate loop_rate(100);
  while (ros::ok())
  {
    if (_pathLoaded)
    {
      monitorPathWaypoints();
    }
    loop_rate.sleep();
    ros::spinOnce();
  }
}
