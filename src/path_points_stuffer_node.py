#!/usr/bin/env python
import rospy
import rospkg
from std_msgs.msg import String
import sys
from os import path
from pathpointsstuffer import PathPointsStuffer
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal

def node():
    rospy.init_node('path_points_stuffer')

    # init path handling object:
    rospack = rospkg.RosPack()
    pth_location_folder = rospack.get_path('agribot_map') + "/paths/"

    points_handler = PathPointsStuffer(pth_location_folder)

    if(not points_handler.set_path_name("testPath")):
        rospy.logerr("[Stuffer test] Path does not exists.")
        return

    points_handler.get_path_points_from_yaml()

    goal_pose = MoveBaseGoal()
    goal_pose.target_pose.header.frame_id = 'map'

    # goal_pose.target_pose.pose.position.x = pose[0][0]
    # goal_pose.target_pose.pose.position.y = pose[0][1]
    # goal_pose.target_pose.pose.position.z = pose[0][2]
    # goal_pose.target_pose.pose.orientation.x = pose[1][0]
    # goal_pose.target_pose.pose.orientation.y = pose[1][1]
    # goal_pose.target_pose.pose.orientation.z = pose[1][2]
    # goal_pose.target_pose.pose.orientation.w = pose[1][3]

    rate = rospy.Rate(0.5)
    while not rospy.is_shutdown():
        rate.sleep()

if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
