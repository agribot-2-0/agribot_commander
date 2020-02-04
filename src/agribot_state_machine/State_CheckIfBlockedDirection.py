#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class CheckIfBlockedDirection(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_no_obstacles'])

    def execute(self, userdata):
        #Check if direction is blocked
        #
        #
        
        string_msg="Executing state CheckIfBlockedDirection "
        rospy.loginfo(string_msg)
        return 'tr_no_obstacles'
