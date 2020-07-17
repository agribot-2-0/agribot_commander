#!/usr/bin/env python
from __future__ import print_function
import rospy
import smach
import roslib
from std_srvs.srv import Empty
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class MobileAppRoutineStart(smach.State):
    # pylint: disable=too-few-public-methods
    """This state decides to which branch of state machine we should transit"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_config_error', 'tr_app_input_received'],
                             input_keys=['mobile_app_routine_reason_in'])

    def execute(self, ud): # pylint: disable=invalid-name, no-self-use
        """Returns the proper transition based on the system configuration"""
        #Send data to mobile app
        #
        #
        output_string = ''
        rospy.loginfo("[State Machine] Executing state: MobileAppRoutineStart INPUT: " +
                      str(ud.mobile_app_routine_reason_in))
        rospy.wait_for_service('/cmd_vel_arbiter_node/switch_to_teleop')
        switch_to_teleop = rospy.ServiceProxy('/cmd_vel_arbiter_node/switch_to_teleop',
                                              Empty)
        try:
            switch_to_teleop()
        except rospy.ServiceException as exc:
            print("Service did not process request: {}".format(exc))
        if ud.mobile_app_routine_reason_in == 'app_input_received':
            output_string = 'tr_app_input_received'
        else:
            output_string = 'tr_task_config_error'
        rospy.loginfo("[State Machine] Leaving state: MobileAppRoutineStart TRANSITION: " +
                      str(output_string))
        return  output_string
