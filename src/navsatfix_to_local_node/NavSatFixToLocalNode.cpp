/*
* Copyright (C) 2020 Agribot. All rights reserved.
*/

#include <agribot_commander/NavSatFixToLocalNode.h>

NavSatFixToLocalNode::NavSatFixToLocalNode():
  _tf2Listener(_tf2Buffer)
{
  _waypointsSub = _nh.subscribe("agribot/waypoints",
                                1000,
                                &NavSatFixToLocalNode::waypointCallback, this);
  _pathPointPub = _nh.advertise<geometry_msgs::PoseStamped>("agribot/path_point",
                                                                1000);

  bool transformStatus = false;

  while  (!transformStatus)
  {
      transformStatus = getMapUTMTransform();
      if (transformStatus == false)
      {
          ROS_WARN("UTM -> map transform was not available. Retrying!");
      }
  }
  ROS_INFO("UTM -> map transform obtained!");
}

bool NavSatFixToLocalNode::getMapUTMTransform(void)
{
    try
    {
        _startingMapUTMTransform = _tf2Buffer.lookupTransform("map",
                                      "utm", ros::Time(0), ros::Duration(1.0));
    }
    catch (tf2::TransformException &ex)
    {
        return false;
    }
    return true;
}

void NavSatFixToLocalNode::waypointCallback(const agribot_msgs::NavSatFixOrientation msg)
{
  // Convert NavSatFix to UTM
  geometry_msgs::PoseStamped UTM_point = navSatFixToUTM(msg);

  // Convert UTM point to map frame
  geometry_msgs::PoseStamped map_point = UTMToMap(UTM_point);

  // Publish converted point
  _pathPointPub.publish(map_point);
}

geometry_msgs::PoseStamped NavSatFixToLocalNode::navSatFixToUTM(
                                              const agribot_msgs::NavSatFixOrientation msg)
{
  double utm_x, utm_y;
  std::string utm_zone;
  geometry_msgs::PoseStamped UTM_point;

  // Convert Latitude/Longitude from NavSatFix to UTM coordinates
  RobotLocalization::NavsatConversions::LLtoUTM(msg.latitude,
                                                msg.longitude,
                                                utm_y, utm_x, utm_zone);

  // Fill in UTM_point variable
  UTM_point.header.frame_id = "utm";
  UTM_point.header.stamp = ros::Time::now();
  UTM_point.pose.position.x = utm_x;
  UTM_point.pose.position.y = utm_y;
  UTM_point.pose.position.z = msg.altitude;
  UTM_point.pose.orientation = msg.orientation;

  return UTM_point;
}

geometry_msgs::PoseStamped NavSatFixToLocalNode::UTMToMap(
                                        const geometry_msgs::PoseStamped msg)
{
  geometry_msgs::PoseStamped map_point;

  // Two alternative ways
  // 1. Get new transform for each new point
  // _tf2Buffer.transform(msg, map_point, "map", ros::Duration(0.2));

  // 2. Use _startingMapUTMTransform generated in constructor
  tf2::doTransform(msg, map_point, _startingMapUTMTransform);
  return map_point;
}

void NavSatFixToLocalNode::run()
{
  ros::spin();
}
