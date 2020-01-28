#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys

# define state Foo
class CheckMobileAppConnection(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['connection_is_stable'])

    def execute(self, userdata):
        rospy.loginfo('Executing state CheckMobileAppConnection')
        return 'connection_is_stable'
