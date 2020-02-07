#!/usr/bin/env python
import smach
import roslib
import rospy
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousTaskMonitor(smach.State):
    """State is used to monitore the task progression, safety,
     internal health of the system, weather conditions, create logs"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_succes'])

    def execute(self, ud):
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rospy.sleep(0.010)
        string_msg = "Executing state AutonomousTaskMonitor "
        rospy.loginfo(string_msg)
        return 'tr_succes'
