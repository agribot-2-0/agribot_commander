// #ifndef AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_
// #define AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_

#include <ros/ros.h>
#include <geometry_msgs/PointStamped.h>

#include <nav_msgs/Path.h>
#include <vector>

class MoveBaseMonitor
{
 private:
  ros::NodeHandle _nh;
  ros::Subscriber _waypointsSub;

  void waypointCallback(const geometry_msgs::PointStampedConstPtr& waypoint);

  // containers
  //!< container for the manually inserted waypoints
  std::vector<geometry_msgs::PoseStamped> waypoints_;
  nav_msgs::Path path_;  //!< container for the generated interpolated path

  // flags
  // !< flag indicating that the waypoint container must be cleared
  // to start anew
  bool clear_waypoints_;

 public:
  MoveBaseMonitor();
  void run();
};

// #endif  // AGRIBOT_COMMANDER_INCLUDE_NAVSATFIXTOLOCALNODE_H_
