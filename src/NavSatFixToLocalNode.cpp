#include <NavSatFixToLocalNode.h>
#include <navsat_conversions.h>

NavSatFixToLocalNode::NavSatFixToLocalNode():
  _tf2Listener(_tf2Buffer)
{
  _waypointsSub = _nh.subscribe("agribot/waypoints", 1000, &NavSatFixToLocalNode::waypointCallback, this);
  _clickedPointPub = _nh.advertise<geometry_msgs::PointStamped>("clicked_point", 1000);

  _startingTransform = _tf2Buffer.lookupTransform("map", "utm", ros::Time(0), ros::Duration(1.0));
}

void NavSatFixToLocalNode::waypointCallback(const sensor_msgs::NavSatFix msg)
{
  // Convert NavSatFix to UTM
  geometry_msgs::PointStamped UTM_point = navSatFixToUTM(msg);

  // Convert UTM point to map frame
  geometry_msgs::PointStamped map_point = UTMToMap(UTM_point);

  // Publish converted point
  _clickedPointPub.publish(map_point);
}

geometry_msgs::PointStamped NavSatFixToLocalNode::navSatFixToUTM(const sensor_msgs::NavSatFix msg)
{
  double utm_x, utm_y;
  std::string utm_zone;
  geometry_msgs::PointStamped UTM_point;

  // Convert Latitude/Longitude from NavSatFix to UTM coordinates
  RobotLocalization::NavsatConversions::LLtoUTM(msg.latitude, msg.longitude, utm_y, utm_x, utm_zone);

  // Fill in UTM_point variable
  UTM_point.header.frame_id = "utm";
  UTM_point.header.stamp = ros::Time::now();
  UTM_point.point.x = utm_x;
  UTM_point.point.y = utm_y;
  UTM_point.point.z = msg.altitude;

  return UTM_point;
}

geometry_msgs::PointStamped NavSatFixToLocalNode::UTMToMap(const geometry_msgs::PointStamped msg)
{
  geometry_msgs::PointStamped map_point;

  // Two alternative ways
  // 1. Get new transform for each new point
  // _tf2Buffer.transform(msg, map_point, "map", ros::Duration(0.2));

  // 2. Use _startingTransform generated in constructor
  tf2::doTransform(msg, map_point, _startingTransform);

  return map_point;
}

void NavSatFixToLocalNode::run()
{
  ros::spin();
}
