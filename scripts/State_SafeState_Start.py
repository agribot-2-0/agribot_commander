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
        smach.State.__init__(self, outcomes=['tr_in_system_power_up', 'tr_in_return_to_safe_state'],
                            input_keys=['safe_state_reason'])

    def execute(self, userdata):
        output_string=''
        #Send data to mobile app
        #
        #
        if userdata.safe_state_reason=='return_to_safe_state':
            string_msg="Executing state SafeStateStart "
            rospy.loginfo(string_msg)
            output_string='tr_in_return_to_safe_state'
        else:
            output_string='tr_in_system_power_up'

        return output_string
