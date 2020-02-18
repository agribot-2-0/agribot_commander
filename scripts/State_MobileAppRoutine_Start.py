#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class MobileAppRoutineStart(smach.State):
    """This state decides to which branch of state machine we should transit"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_config_error', 'tr_app_input_received'],
                             input_keys=['mobile_app_routine_reason_in'])

    def execute(self, ud):
        """Returns the proper transition based on the system configuration"""
        #Send data to mobile app
        #
        #
        output_string = ''
        string_msg = "Executing state machine MobileAppConnection with input "
        string_msg = string_msg + ud.mobile_app_routine_reason_in
        if ud.mobile_app_routine_reason_in == 'app_input_received':
            rospy.loginfo(string_msg)
            output_string = 'tr_app_input_received'
        else:
            rospy.loginfo(string_msg)
            output_string = 'tr_task_config_error'
        return  output_string
