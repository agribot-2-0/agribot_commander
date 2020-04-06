#include <MoveBaseMonitor.h>

#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>
        MoveBaseClient;


MoveBaseMonitor::MoveBaseMonitor()
{
  _waypointsSub = _nh.subscribe("/clicked_point",
                                1000,
                                &MoveBaseMonitor::waypointCallback,
                                this);
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
    // tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);
    move_base_msgs::MoveBaseGoal goal;
    // wait for the action server to come up
    while (!ac.waitForServer(ros::Duration(5.0)))
    {
      ROS_INFO("Waiting for the move_base action server to come up");
    }

    ROS_INFO_STREAM("[MoveBaseMonitor] Start sending goals");
    p2->orientation = p1->orientation;
    path_.header = waypoint->header;
    path_.poses.clear();
    path_.poses.insert(path_.poses.end(), waypoints_.begin(), waypoints_.end());
    for (auto pose : path_.poses)
    {
      ROS_INFO_STREAM("[MoveBaseMonitor] Point " << pose.pose.position.x
                          << ", " << pose.pose.position.y);

      // _clickedPointPub.publish(pose);
      // goal_pub_.publish(pose);
      goal.target_pose = pose;
      ROS_INFO("[MoveBaseMonitor] sendGoal");
      ac.sendGoal(goal);
      ROS_INFO("[MoveBaseMonitor] waitForResult");
      ac.waitForResult();

      if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO_STREAM("[MoveBaseMonitor] Hooray, the base moved" <<
                        "1 meter forward");
      else
        ROS_INFO_STREAM("[MoveBaseMonitor] The base failed to move" <<
                        "forward 1 meter for some reason");
    }
    clear_waypoints_ = true;
    ROS_INFO("[NavSatFixToLocalNode] All goals send");
  }
}

void MoveBaseMonitor::run()
{
  ros::spin();
}
