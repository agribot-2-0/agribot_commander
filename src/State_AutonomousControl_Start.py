#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class AutonomousControlStart(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['auto_control_prepared', 'sys_repaired'],
                            input_keys=['autonomous_control_state_reason'])

    def execute(self, userdata):
        #Send data to mobile app
        #
        #
        if userdata.autonomous_control_state_reason=='auto_control_prepared':
            string_msg="Executing state AutonomousControlStart "
            rospy.loginfo(string_msg)
            return 'auto_control_prepared'
        else:
             return 'sys_repaired'

