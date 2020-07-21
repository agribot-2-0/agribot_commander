/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#pragma once

#include <ros/ros.h>
#include <geometry_msgs/PointStamped.h>
#include <std_msgs/Int32.h>
#include <agribot_msgs/AgribotMoveBaseMonitorResult.h>
#include <agribot_msgs/AgribotMoveBaseCommand.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <nav_msgs/Path.h>
#include <vector>
#include <string>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>
        MoveBaseClient;

class MoveBaseMonitor
{
 private:
  ros::NodeHandle _nh;
  ros::Subscriber _waypointsSub;
  ros::Subscriber _stateMachineSub;
  ros::Publisher _stateMachinePub;
  MoveBaseClient* ac;
  ros::Publisher waypoint_marker_pub_;
  ros::Publisher plan_pub_;

  void createAndPublishMarkersFromPath(const std::vector <geometry_msgs::PoseStamped>& path); // NOLINT

  void waypointCallback(const geometry_msgs::PointStampedConstPtr& waypoint);

  void stateMachineCallback(const agribot_msgs::AgribotMoveBaseCommand &command_msg);

  void isPaused();

  void monitorPathWaypoints();
  // containers
  //!< container for the manually inserted waypoints
  std::vector<geometry_msgs::PoseStamped> waypoints_;
  nav_msgs::Path path_;  //!< container for the generated interpolated path

  // flags
  // !< flag indicating that the waypoint container must be cleared
  // to start anew
  bool clear_waypoints_;
  bool _pathLoaded;
  bool _pauseGoal;
  bool _pauseGoalLatch;
  bool _userAbort;
  agribot_msgs::AgribotMoveBaseMonitorResult resultMsg;

 public:
  MoveBaseMonitor();
  void run();
};
