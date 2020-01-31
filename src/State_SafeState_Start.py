#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class SafeStateStart(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['in_system_power_up', 'in_return_to_safe_state'],
                            input_keys=['safe_state_reason'])

    def execute(self, userdata):
        #Send data to mobile app
        #
        #
        if userdata.safe_state_reason=='return_to_safe_state':
            string_msg="Executing state SafeStateStart "
            rospy.loginfo(string_msg)
            return 'in_return_to_safe_state'
        else:
             return 'in_system_power_up'

