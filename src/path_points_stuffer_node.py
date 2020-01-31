#!/usr/bin/env python
import rospy
import rospkg
import actionlib
from std_msgs.msg import String
import sys
from os import path
from pathpointsstuffer import PathPointsStuffer
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
#from geometry_msgs.msg import PointStamped

def node():
    rospy.init_node('path_points_stuffer')
    #pub = rospy.Publisher('/clicked_point', PointStamped, queue_size=10)

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

    ###### action lib approach:
    goal_pose = MoveBaseGoal()
    goal_pose.target_pose.header.frame_id = 'map'

    #goal_pose = PointStamped()
    #goal_pose.header.frame_id = "odom"
    send_last_msg = True
    rate = rospy.Rate(1000)

    while not rospy.is_shutdown():
        if(points_handler.are_path_points_availible()):
            ###### action lib approach:
            goal_pose = points_handler.fill_new_msg(goal_pose)
            client.send_goal(goal_pose)
            rospy.loginfo("Action state {}".format(client.get_state()))
            client.wait_for_result()
            ###### HAKSY: public into /clicked_point
            #goal_pose.header.stamp = rospy.get_rostime()
            #goal_pose = points_handler.fill_new_msg_point(goal_pose)
            #pub.publish(goal_pose)
        else:
            if(send_last_msg):
                #pub.publish(goal_pose)
                send_last_msg = False
        rate.sleep()

if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
