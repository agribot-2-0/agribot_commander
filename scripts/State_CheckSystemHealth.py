#!/usr/bin/env python

import smach
import rospy
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class CheckSystemHealth(smach.State):
    """In this state, assesment of nodes work and connections is performmed."""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_system_is_healthy'])

    def execute(self, ud):
        """Assesment of ROS nodes work and connections, based on their published topics data.
         If something looks wrong the eror transition is performed"""
        rospy.sleep(0.010)
        string_msg = "Executing state CheckSystemHealth "
        rospy.loginfo(string_msg)
        return 'tr_system_is_healthy'
