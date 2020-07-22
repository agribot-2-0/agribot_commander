/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#ifndef AGRIBOT_COMMANDER_NAVSATFIXTOLOCALNODE_H_
#define AGRIBOT_COMMANDER_NAVSATFIXTOLOCALNODE_H_

#include <ros/ros.h>
#include <tf2/utils.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <sensor_msgs/NavSatFix.h>
#include <agribot_msgs/NavSatFixOrientation.h>
#include <geometry_msgs/PoseStamped.h>
#include <robot_localization/navsat_conversions.h>

class NavSatFixToLocalNode
{
 private:
  ros::NodeHandle _nh;
  ros::Subscriber _waypointsSub;
  ros::Publisher _pathPointPub;
  tf2_ros::Buffer _tf2Buffer;
  tf2_ros::TransformListener _tf2Listener;

  geometry_msgs::TransformStamped _startingMapUTMTransform;

  bool getMapUTMTransform(void);
  void waypointCallback(const agribot_msgs::NavSatFixOrientation msg);
  geometry_msgs::PoseStamped navSatFixToUTM(const agribot_msgs::NavSatFixOrientation msg);
  geometry_msgs::PoseStamped UTMToMap(const geometry_msgs::PoseStamped msg);


 public:
  NavSatFixToLocalNode();
  void run();
};

#endif  // AGRIBOT_COMMANDER_NAVSATFIXTOLOCALNODE_H_
