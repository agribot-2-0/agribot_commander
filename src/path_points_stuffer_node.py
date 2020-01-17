#!/usr/bin/env python
import rospy
import rospkg
import actionlib
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
    points_handler.transform_to_robot_coordinates()

    #init actionlib:
    client = actionlib.SimpleActionClient('move_base', MoveBaseAction)
    client.wait_for_server()

    goal_pose = MoveBaseGoal()
    goal_pose.target_pose.header.frame_id = 'map'

    rate = rospy.Rate(0.1)
    while not rospy.is_shutdown():
        if(points_handler.are_path_points_availible()):
            goal_pose = points_handler.fill_new_msg(goal_pose)

            ###### action lib approach:
            client.send_goal(goal_pose)
            rospy.loginfo("Action state {}".format(client.get_state()))
            client.wait_for_result()
            ###### TODO: public /clicked_point (msg type: geometry_msgs/Point):

        else:
            rospy.loginfo("Path finished!")
        rate.sleep()

if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
