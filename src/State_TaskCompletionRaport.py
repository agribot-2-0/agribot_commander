#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class TaskCompletionRaport(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['complete'])

    def execute(self, userdata):
        #Write raport data and go back to Standby
        #
        #
        string_msg="Executing state TaskCompletionRaport "
        rospy.loginfo(string_msg)
        return 'complete'
