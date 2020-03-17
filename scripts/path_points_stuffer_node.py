#!/usr/bin/env python
from os import path
from sensor_msgs.msg import NavSatFix
import rospy
import rospkg
from std_msgs.msg import String
from pathpointsstuffer import PathPointsStuffer


class PathPointsStufferNode(object):
    def __init__(self):
        self.received_new_task = False
        self.new_task_name = ""

        rospy.init_node('path_points_stuffer_node')

        rospack = rospkg.RosPack()
        path_location_folder = rospack.get_path('agribot_map') + "/paths/"
        self.points_stuffer = PathPointsStuffer(path_location_folder)

        self.pub = rospy.Publisher('/agribot/waypoints', NavSatFix, queue_size=1000)
        self.sub = rospy.Subscriber('/agribot/chosen_task_name', String, self.callback_chosentask)

    def callback_chosentask(self, msg):
        self.new_task_name = msg.data
        self.received_new_task = True

    def run(self):
        send_last_msg = False
        path_loaded = False

        rate = rospy.Rate(1000)

        while not rospy.is_shutdown():

            if self.received_new_task:
                if not self.points_stuffer.set_path_name(self.new_task_name):
                    rospy.logerr("[Stuffer test] Path " + self.new_task_name + "does not exists.")
                    send_last_msg = False
                    self.received_new_task = False
                    path_loaded = False
                else:
                    self.points_stuffer.get_path_points_from_yaml()
                    goal_fix = NavSatFix()
                    goal_fix.header.frame_id = "ll"
                    send_last_msg = True
                    self.received_new_task = False
                    path_loaded = True
            if path_loaded:
                if self.points_stuffer.are_path_points_availible():
                    goal_fix.header.stamp = rospy.get_rostime()
                    goal_fix = self.points_stuffer.fill_new_msg_fix(goal_fix)
                    self.pub.publish(goal_fix)
                else:
                    if send_last_msg:
                        rospy.loginfo("Path " + self.new_task_name + " loaded succesfully")
                        self.pub.publish(goal_fix)
                        send_last_msg = False
                        path_loaded = False
            rate.sleep()

if __name__ == '__main__':
    try:
        pps_node = PathPointsStufferNode()
        pps_node.run()
    except rospy.ROSInterruptException:
        pass
