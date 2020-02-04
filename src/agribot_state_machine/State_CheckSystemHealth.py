#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class CheckSystemHealth(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_system_is_healthy'])

    def execute(self, userdata):
        rospy.sleep(0.010)
        string_msg="Executing state CheckSystemHealth "
        rospy.loginfo(string_msg)
        return 'tr_system_is_healthy'
