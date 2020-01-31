#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class AutonomousTaskMonitor(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['succes'])

    def execute(self, userdata):
        #Monitore the task progression, safety, internal health of the system, weather conditions, create logs
        #
        #
        rospy.sleep(0.010)
        string_msg="Executing state AutonomousTaskMonitor "
        rospy.loginfo(string_msg)
        return 'succes'
