#!/usr/bin/env python
from __future__ import print_function
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class ManualControlStart(smach.State):
    # pylint: disable=too-few-public-methods
    """This state is an entry state of state machine and decides which branch to pick depdending
    on the data input received"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_config_error', 'tr_app_input_received'],
                             input_keys=['manual_control_state_reason'],
                             output_keys=['manual_control_state_app_reason'])

    def execute(self, ud): # pylint: disable=invalid-name, no-self-use
        """Returns the proper transition depending on the system situation during entry to the
        statemachine, described by userdata (ud)"""
        #Send data to mobile app
        #
        #
        output_string = ''
        if ud.manual_control_state_reason == 'application_input_received':
            ud.manual_control_state_app_reason = 'app_input_received'
            string_msg = "[State Machine] Executing state ManualControlStart "
            rospy.loginfo(string_msg)
            output_string = 'tr_app_input_received'
        else:
            ud.manual_control_state_app_reason = 'task_config_error'
            output_string = 'tr_task_config_error'
        return output_string
