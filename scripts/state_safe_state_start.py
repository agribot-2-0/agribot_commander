#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class SafeStateStart(smach.State):
    # pylint: disable=too-few-public-methods
    """This is an entrance state used to decide which branch of state machine should be picked"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_in_system_power_up',
                                             'tr_in_return_to_safe_state'],
                             input_keys=['safe_state_reason'])

    def execute(self, ud): # pylint: disable=invalid-name, no-self-use
        """Decide which transition to pick based on the userdata (ud)"""
        output_transition = ''
        rospy.loginfo("[State Machine] Executing state: SafeStateStart INPUT: " +
                      str(ud.safe_state_reason))
        # we dont have the failure reaction yet, but here is the place to start transition
        # to critical failure state where we will start reboot/relaunch routine etc
        if ud.safe_state_reason == 'return_to_safe_state' or \
           ud.safe_state_reason == 'return_to_safe_state_failure':
            output_transition = 'tr_in_return_to_safe_state'
            rospy.set_param('/agribot/robot_current_state', 'safe_state')
        else:
            output_transition = 'tr_in_system_power_up'
            rospy.set_param('/agribot/robot_current_state', 'safe_state')
        rospy.loginfo("[State Machine] Leaving state: SafeStateStart TRANSITION: " +
                      str(output_transition))
        return output_transition
