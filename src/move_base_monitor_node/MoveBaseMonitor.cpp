#include <MoveBaseMonitor.h>

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
  _stateMachinePub = _nh.advertise<std_msgs::Int32>
                      ("/agribot/move_base_monitor/result", 10);
  // tell the action client that we want to spin a thread by default
  ac = new MoveBaseClient("move_base", true);
  // wait for the action server to come up
  while (!ac->waitForServer(ros::Duration(5.0)))
  {
    ROS_INFO("[MoveBaseMonitor]Waiting for the move_base action server to come up");
  }

  resultMsg.data = 0;
  _pauseGoal = false;
}

void MoveBaseMonitor::stateMachineCallback(const std_msgs::Int32 &command)
{
  //if the path is blocked then cancel the current goal and set the pause flag
  if (command.data == 1)
  {
    ac->cancelAllGoals();
    _pauseGoal = true;
  } 
  else
  {
    _pauseGoal = false;
  }
}

void MoveBaseMonitor::isPaused()
{
  bool cancelTrig = true;
  while(_pauseGoal)
  {
    if(cancelTrig)
    {
      ac->cancelAllGoals();
      cancelTrig = false;
    }
    ros::Duration(0.5).sleep(); // sleep for half a second
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
    move_base_msgs::MoveBaseGoal goal;
    actionlib::SimpleClientGoalState::StateEnum goal_result;

    ROS_INFO_STREAM("[MoveBaseMonitor] Start sending goals");
    p2->orientation = p1->orientation;
    path_.header = waypoint->header;
    path_.poses.clear();
    path_.poses.insert(path_.poses.end(), waypoints_.begin(), waypoints_.end());
    //auto last_pose = path_.poses.begin();
    int currentWaypoint = 0;

    auto pose = path_.poses[currentWaypoint];
    while(currentWaypoint < path_.poses.size())
    {
      //check if the current goal was pause, if so then wait
      isPaused();

      pose = path_.poses[currentWaypoint];
      ROS_INFO_STREAM("[MoveBaseMonitor] Point " << pose.pose.position.x
                          << ", " << pose.pose.position.y);
      goal.target_pose = pose;
      ROS_INFO("[MoveBaseMonitor] sendGoal");
      ac->sendGoal(goal);
      ROS_INFO("[MoveBaseMonitor] waitForResult");
      ac->waitForResult();
      goal_result = ac->getState().state_;
      ROS_INFO_STREAM("[MoveBaseMonitor] Result: " << goal_result);
      if (goal_result == actionlib::SimpleClientGoalState::SUCCEEDED)
      {
        resultMsg.data = 1;
        _stateMachinePub.publish(resultMsg);
        ROS_INFO_STREAM("[MoveBaseMonitor] Goal reached");
        currentWaypoint++;
      }
      else if (goal_result == actionlib::SimpleClientGoalState::PREEMPTED)
      {
        //check if the current goal is paused or we should take next goal
        if(_pauseGoal)
        {
          resultMsg.data = 2;
          _stateMachinePub.publish(resultMsg);
          ROS_INFO_STREAM("[MoveBaseMonitor] Goal paused");
        } 
        else
        {
          resultMsg.data = 4;
          _stateMachinePub.publish(resultMsg);
          currentWaypoint++;
          ROS_INFO_STREAM("[MoveBaseMonitor] Goal canceled");
        }
      }
      else
      {
        resultMsg.data = 10;
        _stateMachinePub.publish(resultMsg);
        currentWaypoint++;
        ROS_INFO_STREAM("[MoveBaseMonitor] The base failed to move, trying next goal");
      }
    }
    resultMsg.data = 100;
    _stateMachinePub.publish(resultMsg);
    clear_waypoints_ = true;
    ROS_INFO("[NavSatFixToLocalNode] All goals send");
  }
}

void MoveBaseMonitor::run()
{
  ros::spin();
}
