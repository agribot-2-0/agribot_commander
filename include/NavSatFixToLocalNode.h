#ifndef AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_
#define AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_

#include <ros/ros.h>
#include <tf2/utils.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PointStamped.h>
#include <robot_localization/navsat_conversions.h>

class NavSatFixToLocalNode
{
 private:
  ros::NodeHandle _nh;
  ros::Subscriber _waypointsSub;
  ros::Publisher _clickedPointPub;
  tf2_ros::Buffer _tf2Buffer;
  tf2_ros::TransformListener _tf2Listener;

  geometry_msgs::TransformStamped _startingMapUTMTransform;

  bool getMapUTMTransform(void);
  void waypointCallback(const sensor_msgs::NavSatFix msg);
  geometry_msgs::PointStamped navSatFixToUTM(const sensor_msgs::NavSatFix msg);
  geometry_msgs::PointStamped UTMToMap(const geometry_msgs::PointStamped msg);


 public:
  NavSatFixToLocalNode();
  void run();
};

#endif  // AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_
