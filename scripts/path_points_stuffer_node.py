#!/usr/bin/env python
from os import path
from sensor_msgs.msg import NavSatFix
import rospy
import rospkg
from std_msgs.msg import String
from pathpointsstuffer import PathPointsStuffer

received_new_task = False
new_task_name = ""

def callback_choosentask(data):
    global received_new_task
    global new_task_name
    rospy.loginfo("Dostalem "+str(data.data))
    new_task_name = str(data.data)
    received_new_task = True

def node():
    global received_new_task
    global new_task_name

    rospy.init_node('path_points_stuffer_node')
    sub = rospy.Subscriber('/agribot/chosen_task_name', String, callback_choosentask)
    pub = rospy.Publisher('agribot/waypoints', NavSatFix, queue_size=1000)

    received_new_task = False
    send_last_msg = False
    path_loaded = False

    # init path handling object:
    rospack = rospkg.RosPack()
    pth_location_folder = rospack.get_path('agribot_map') + "/paths/"

    points_handler = PathPointsStuffer(pth_location_folder)

    rate = rospy.Rate(1000)

    while not rospy.is_shutdown():
        
        if received_new_task:
            if not points_handler.set_path_name(str(new_task_name)):
                rospy.logerr("[Stuffer test] Path " + str(new_task_name)+ "does not exists.")
                send_last_msg = False
                received_new_task = False
                path_loaded =  False
            else:
                points_handler.get_path_points_from_yaml()
                goal_fix = NavSatFix()
                goal_fix.header.frame_id = "ll"
                send_last_msg = True
                received_new_task = False
                path_loaded =  True
        if path_loaded:
            if points_handler.are_path_points_availible():
                goal_fix.header.stamp = rospy.get_rostime()
                goal_fix = points_handler.fill_new_msg_fix(goal_fix)
                pub.publish(goal_fix)
            else:
                if send_last_msg:
                    rospy.loginfo("Loading path " + str(new_task_name) + " loaded succesfully")
                    pub.publish(goal_fix)
                    send_last_msg = False
                    path_loaded = False
        rate.sleep()

if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
