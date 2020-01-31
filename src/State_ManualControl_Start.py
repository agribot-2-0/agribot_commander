#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class ManualControlStart(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['task_config_error', 'app_input_received'],
                            input_keys=['manual_control_state_reason'],
                            output_keys=['manual_control_state_app_reason'])

    def execute(self, userdata):
        #Send data to mobile app
        #
        #
        if userdata.manual_control_state_reason=='application_input_received':
            userdata.manual_control_state_app_reason='app_input_received'
            string_msg="Executing state ManualControlStart "
            rospy.loginfo(string_msg)
            return 'app_input_received'
        else:
             userdata.manual_control_state_app_reason='task_config_error'
             return 'task_config_error'

