#!/usr/bin/env python
from os import path
import rospy
import rospkg
from std_msgs.msg import String
from pathpointsstuffer import PathPointsStuffer
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
from sensor_msgs.msg import NavSatFix


def node():
    rospy.init_node('path_points_stuffer')
    pub = rospy.Publisher('agribot/waypoints', NavSatFix, queue_size=1000)

    # init path handling object:
    rospack = rospkg.RosPack()
    pth_location_folder = rospack.get_path('agribot_map') + "/paths/"
    points_handler = PathPointsStuffer(pth_location_folder)
    if not points_handler.set_path_name("testPath"):
        rospy.logerr("[Stuffer test] Path does not exists.")
        return
    points_handler.get_path_points_from_yaml()

    #init actionlib:
    #client = actionlib.SimpleActionClient('move_base', MoveBaseAction)
    #client.wait_for_server()

    ###### action lib approach:
    #goal_pose = MoveBaseGoal()
    #goal_pose.target_pose.header.frame_id = 'map'

    goal_fix = NavSatFix()
    goal_fix.header.frame_id = "ll"
    send_last_msg = True
    rate = rospy.Rate(1000)

    while not rospy.is_shutdown():
        if points_handler.are_path_points_availible():
            ###### action lib approach:
            #goal_pose = points_handler.fill_new_msg(goal_pose)
            #client.send_goal(goal_pose)
            #rospy.loginfo("Action state {}".format(client.get_state()))
            #client.wait_for_result()
            ###### HAKSY: public into /clicked_point
            goal_fix.header.stamp = rospy.get_rostime()
            goal_fix = points_handler.fill_new_msg_fix(goal_fix)
            pub.publish(goal_fix)
        else:
            if send_last_msg:
                pub.publish(goal_pose)
            if(send_last_msg):
                pub.publish(goal_fix)
                send_last_msg = False
        rate.sleep()

if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
