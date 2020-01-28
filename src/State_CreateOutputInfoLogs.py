#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class CreateOutputInfoLogs(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['executed_outputs'])

    def execute(self, userdata):
        #Create output info logs
        #
        #
        string_msg="Executing state CreateOutputInfoLogs "
        rospy.loginfo(string_msg)
        return 'executed_outputs'
