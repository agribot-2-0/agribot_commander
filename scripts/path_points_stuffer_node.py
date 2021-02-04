#!/usr/bin/env python
import os
import rospy
from std_msgs.msg import String
from agribot_msgs.msg import NavSatFixOrientation
from agribot_common.agribot_parameters import get_parameter
from pathpointsstuffer import PathPointsStuffer

LOG_TAG = "PathPointsStuffer"


class PathPointsStufferNode(object):

    def __init__(self):
        self.received_new_task = False
        self.new_task_name = ""

        rospy.init_node('path_points_stuffer_node')

        try:
            path_location_folder = os.path.join(os.environ.get('AGRIBOT_HOME'),
                                                get_parameter('/agribot_map/paths_path'))
        except ValueError as exception_msg:
            rospy.logerr("[{}] {}".format(LOG_TAG, exception_msg))
            return

        self.points_stuffer = PathPointsStuffer(path_location_folder)

        self.pub = rospy.Publisher('/agribot/waypoints', NavSatFixOrientation, queue_size=1000)
        self.sub = rospy.Subscriber('/agribot/chosen_task_name', String, self.callback_chosentask)

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
            rospy.loginfo("[{}] Path {} loaded succesfully".format(LOG_TAG, self.new_task_name))
        else:
            rospy.logerr("[{}] Path {} does not exist".format(LOG_TAG, self.new_task_name))

    @staticmethod
    def run():
        rospy.spin()


if __name__ == '__main__':
    try:
        PATH_POINT_STUFFER_NODE = PathPointsStufferNode()
        PATH_POINT_STUFFER_NODE.run()
    except rospy.ROSInterruptException:
        pass
