#!/usr/bin/env python
from __future__ import print_function
import rospy
import smach
import roslib
from std_srvs.srv import Empty
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousControlStart(smach.State):
    # pylint: disable=too-few-public-methods
    """This state is an entry state and its purpose is to decide which branch of state machine
     should be picked based on the conditions of entry, described by userdata"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_auto_control_prepared', 'tr_sys_repaired'],
                             input_keys=['autonomous_control_state_reason'])

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument, no-self-use
        """Returns the proper transition based on the input of userdata (ud)"""
        output_transition = ''
        #Send data to mobile app
        rospy.wait_for_service('/cmd_vel_arbiter_node/switch_to_navigation')
        switch_to_navigation = rospy.ServiceProxy('/cmd_vel_arbiter_node/switch_to_navigation',
                                                  Empty)
        try:
            switch_to_navigation()
        except rospy.ServiceException as exc:
            rospy.logerr("Service did not process request: " + str(exc))
        rospy.loginfo("[State Machine] Executing state: AutonomousControlStart INPUT: " +
                      str(ud.autonomous_control_state_reason))
        if ud.autonomous_control_state_reason == 'auto_control_prepared' or \
           ud.autonomous_control_state_reason == 'autonomous_config_present':
            output_transition = 'tr_auto_control_prepared'
        elif ud.autonomous_control_state_reason == 'system_repaired':
            rospy.loginfo("[State Machine] Continuing state AutonomousControlStart ")
            output_transition = 'tr_sys_repaired'
        else:
            output_transition = 'tr_sys_repaired'
        rospy.loginfo("[State Machine] Leaving state: AutonomousControlStart TRANSITION: " +
                      str(output_transition))
        return output_transition
