/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#include <agribot_commander/MoveBaseMonitor.h>
#include <visualization_msgs/MarkerArray.h>

#define COMMAND_CONTINUE 0
#define COMMAND_PAUSE 1
#define COMMAND_ABORT 2

#define RESULT_ACTIVE 1
#define RESULT_SUCCES 3
#define RESULT_PREEMPTED 2
#define RESULT_ABORTED 4
#define RESULT_FAILED 5
#define RESULT_PAUSED 10
#define RESULT_PATH_COMPLETED 100

MoveBaseMonitor::MoveBaseMonitor()
{
  _waypointsSub = _nh.subscribe("/clicked_point",
                                1000,
                                &MoveBaseMonitor::waypointCallback,
                                this);
  _stateMachineSub = _nh.subscribe("/agribot/move_base_monitor/command",
                                   10,
                                   &MoveBaseMonitor::stateMachineCallback,
                                   this);
  _stateMachinePub = _nh.advertise<agribot_msgs::AgribotMoveBaseMonitorResult>
                      ("/agribot/move_base_monitor/result", 10);

  waypoint_marker_pub_ = _nh.advertise<visualization_msgs::MarkerArray>
                        ("/agribot/move_base_monitor/waypoints", 1);
  plan_pub_ = _nh.advertise<nav_msgs::Path>("/agribot/move_base_monitor/path",
                                             1);
  // tell the action client that we want to spin a thread by default
  ac = new MoveBaseClient("move_base", true);
  // wait for the action server to come up
  while (!ac->waitForServer(ros::Duration(5.0)))
  {
    ROS_INFO_STREAM("[MoveBaseMonitor]Waiting for the move_base" <<
                    "action server to come up");
  }

  resultMsg.result = 0;
  resultMsg.current_waypoint = 0;
  resultMsg.total_waypoints = 0;
  _pauseGoal = false;
  _pauseGoalLatch = false;
  _pathLoaded = false;
  _userAbort = false;
}

void MoveBaseMonitor::stateMachineCallback(const std_msgs::Int32 &command)
{
  // if the path is blocked then cancel the current goal and set the pause flag
  if (command.data == COMMAND_PAUSE)
  {
    ac->cancelAllGoals();
    _pauseGoal = true;
    _pauseGoalLatch = true;
  }
  else if (command.data == COMMAND_CONTINUE)
  {
    _pauseGoal = false;
  }
  // if we want to abort the current task by user request
  else if (command.data == COMMAND_ABORT)
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
    resultMsg.result = RESULT_PAUSED;
    _stateMachinePub.publish(resultMsg);
    ros::Duration(0.1).sleep();  // sleep for half a second
    ros::spinOnce();
  }
}

void MoveBaseMonitor::waypointCallback(
          const geometry_msgs::PointStampedConstPtr& waypoint)
{
  if (clear_waypoints_)
  {
    waypoints_.clear();
    clear_waypoints_ = false;
  }

  // add waypoint to the waypoint vector
  waypoints_.push_back(geometry_msgs::PoseStamped());
  waypoints_.back().header = waypoint->header;
  waypoints_.back().pose.position = waypoint->point;
  waypoints_.back().pose.orientation.w = 1.0;

  if (waypoints_.size() < 2)
    return;

  geometry_msgs::Pose *p1 = &(waypoints_.end()-2)->pose;
  geometry_msgs::Pose *p2 = &(waypoints_.end()-1)->pose;

  // calculate orientation of waypoints
  double yaw = atan2(p2->position.y - p1->position.y,
                     p2->position.x - p1->position.x);
  p1->orientation.w = 1;

  // calculate distance between latest two waypoints
  // and check if it surpasses the threshold epsilon
  double dist = hypot(p1->position.x - p2->position.x,
                      p1->position.y - p2->position.y);

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

void MoveBaseMonitor::createAndPublishMarkersFromPath(
  const std::vector<geometry_msgs::PoseStamped>& path)
{
  // clear previous markers
  visualization_msgs::MarkerArray markers;
  visualization_msgs::Marker marker;
  marker.header = path[0].header;
  marker.ns = "/move_base_monitor/waypoints";
  marker.type = visualization_msgs::Marker::SPHERE;
  marker.action = visualization_msgs::Marker::DELETEALL;
  marker.scale.x = 0.4;
  marker.scale.y = 0.4;
  marker.scale.z = 0.4;
  marker.color.a = 1.0;
  marker.color.r = 0.0;
  marker.color.g = 0.0;
  marker.color.b = 1.0;
  marker.id = 0;
  markers.markers.push_back(marker);
  waypoint_marker_pub_.publish(markers);
  marker.action = visualization_msgs::Marker::ADD;
  markers.markers.clear();

  for (size_t i = 0; i < path.size(); i++)
  {
    marker.id = i;
    marker.pose.position = path[i].pose.position;
    marker.pose.orientation.w = 1;
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
      ROS_INFO_STREAM("[MoveBaseMonitor] Point " << pose.pose.position.x
              << ", " << pose.pose.position.y << " Index " << currentWaypoint);
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
        resultMsg.result = RESULT_ACTIVE;
        _stateMachinePub.publish(resultMsg);
      }
      else if (goal_result == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        resultMsg.result = RESULT_SUCCES;
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
          resultMsg.result = RESULT_PAUSED;
          _stateMachinePub.publish(resultMsg);
          load_point = true;
          _pauseGoalLatch = false;
        }
        else
        {
          resultMsg.result = RESULT_PREEMPTED;
          _stateMachinePub.publish(resultMsg);
          ROS_INFO_STREAM("[MoveBaseMonitor] Goal PREEMPTED");
          currentWaypoint++;
          load_point = true;
        }
      }
      else if (goal_result == actionlib::SimpleClientGoalState::ABORTED)
      {
        resultMsg.result = RESULT_ABORTED;
        _stateMachinePub.publish(resultMsg);
        currentWaypoint++;
        load_point = true;
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was ABORTED!," <<
                        "trying next goal");
      }
      else if (goal_result == actionlib::SimpleClientGoalState::REJECTED)
      {
        resultMsg.result = RESULT_FAILED;
        _stateMachinePub.publish(resultMsg);
        currentWaypoint++;
        load_point = true;
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was REJECTED!," <<
                        "trying next goal");
      }
      else if (goal_result == actionlib::SimpleClientGoalState::RECALLED)
      {
        resultMsg.result = RESULT_FAILED;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was RECALLED!," <<
                        "trying next goal");
        currentWaypoint++;
        load_point = true;
      }
      else if (goal_result == actionlib::SimpleClientGoalState::LOST)
      {
        resultMsg.result = RESULT_FAILED;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] The goal was LOST!," <<
                        "trying next goal");
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
    resultMsg.result = RESULT_PATH_COMPLETED;
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
