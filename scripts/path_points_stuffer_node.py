#!/usr/bin/env python
import rospy
import rospkg
from std_msgs.msg import String
from sensor_msgs.msg import NavSatFix
from agribot_msgs.msg import NavSatFixOrientation
from pathpointsstuffer import PathPointsStuffer


class PathPointsStufferNode(object):
    def __init__(self):
        self.received_new_task = False
        self.new_task_name = ""

        rospy.init_node('path_points_stuffer_node')

        rospack = rospkg.RosPack()
        path_location_folder = rospack.get_path('agribot_map') + "/paths/"
        self.points_stuffer = PathPointsStuffer(path_location_folder)

        self.pub = rospy.Publisher('/agribot/waypoints',
                                   NavSatFixOrientation,
                                   queue_size=1000)
        self.sub = rospy.Subscriber('/agribot/chosen_task_name',
                                    String,
                                    self.callback_chosentask)

    def callback_chosentask(self, msg):
        self.new_task_name = msg.data
        if self.points_stuffer.set_path_name(self.new_task_name):
            goal_fix = NavSatFixOrientation()
            goal_fix.header.frame_id = "ll"
            self.points_stuffer.get_path_points_from_yaml()

            # Publish path
            while self.points_stuffer.are_path_points_availible():
                goal_fix.header.stamp = rospy.get_rostime()
                goal_fix = self.points_stuffer.fill_new_msg_fix(goal_fix)
                self.pub.publish(goal_fix)

            # Path finished - repeat last waypoint
            self.pub.publish(goal_fix)
            rospy.loginfo("[PathPointsStuffer] Path {} loaded succesfully"
                          .format(self.new_task_name))
        else:
            rospy.logerr("[PathPointsStuffer] Path {} does not exist"
                         .format(self.new_task_name))

    @staticmethod
    def run():
        rospy.spin()


if __name__ == '__main__':
    try:
        PATH_POINT_STUFFER_NODE = PathPointsStufferNode()
        PATH_POINT_STUFFER_NODE.run()
    except rospy.ROSInterruptException:
        pass
