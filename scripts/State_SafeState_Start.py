#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class SafeStateStart(smach.State):
    """This is an entrance state used to decide which branch of state machine should be picked"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_in_system_power_up', 'tr_in_return_to_safe_state'],
                             input_keys=['safe_state_reason'])

    def execute(self, ud):
        """Decide which transition to pick based on the userdata (ud)"""
        output_string = ''
        #Send data to mobile app
        #
        #
        if ud.safe_state_reason == 'return_to_safe_state':
            string_msg = "Executing state SafeStateStart "
            rospy.loginfo(string_msg)
            output_string = 'tr_in_return_to_safe_state'
        else:
            output_string = 'tr_in_system_power_up'
        return output_string
