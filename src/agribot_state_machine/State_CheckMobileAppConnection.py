#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys

# define state Foo
class CheckMobileAppConnection(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_stable'])

    def execute(self, userdata):
        rospy.sleep(0.010)
        rospy.loginfo('Executing state CheckMobileAppConnection')
        return 'tr_connection_is_stable'
