#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousControlStart(smach.State):
    """This state is an entry state and its purpose is to decide which branch of state machine
     should be picked based on the conditions of entry, described by userdata"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_auto_control_prepared', 'tr_sys_repaired'],
                             input_keys=['autonomous_control_state_reason'])

    def execute(self, ud):
        """Returns the proper transition based on the input of userdata (ud)"""
        output_string = ''
        #Send data to mobile app
        #
        #
        if ud.autonomous_control_state_reason == 'auto_control_prepared' or ud.autonomous_control_state_reason == 'autonomous_config_present':
            string_msg = "Executing state AutonomousControlStart "
            rospy.loginfo(string_msg)
            output_string = 'tr_auto_control_prepared'
        elif ud.autonomous_control_state_reason == 'system_repaired':
            string_msg = "Continuing state AutonomousControlStart "
            rospy.loginfo(string_msg)
            output_string = 'tr_sys_repaired'
        else:
            output_string = 'tr_sys_repaired'
        return output_string
