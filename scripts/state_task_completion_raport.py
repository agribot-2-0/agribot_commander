#!/usr/bin/env python
import rospy
import smach
import roslib
from std_msgs.msg import String
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


class TaskCompletionRaport(smach.State):
    # pylint: disable=too-few-public-methods
    """State is used to create a summary report from from the Agribots work"""

    def __init__(self):
        smach.State.__init__(
            self,
            outcomes=['tr_complete_return_to_safe_state', 'tr_suspended_return_to_manual'],
            output_keys=['task_completion_safe_out', 'task_completion_man_out'],
            input_keys=['auto_task_monitor_input', 'auto_task_blocked_input'])
        self.pub_state = rospy.Publisher('/agribot/state_machine/state', String, queue_size=10)
        self.state_msg = String()

    def execute(self, ud):  # pylint: disable=invalid-name, no-self-use
        """Create a report from the task realisation"""
        #Write raport data and go back to SafeState or ManualControl
        #
        #
        self.state_msg = "TaskCompletionRaport"
        self.pub_state.publish(self.state_msg)
        current_state = rospy.get_param('/agribot/robot_current_state')
        output_transition = ''
        if current_state == 'auto_control_state':
            rospy.loginfo("[StateMachine] Executing state: TaskCompletionRaport INPUT: " +
                          str(ud.auto_task_monitor_input))
            if ud.auto_task_monitor_input == 'user_abort':
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_transition = 'tr_complete_return_to_safe_state'

            if ud.auto_task_monitor_input == 'user_suspend':
                ud.task_completion_man_out = 'application_input_received'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_transition = 'tr_suspended_return_to_manual'

            if ud.auto_task_monitor_input == 'succes':
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_transition = 'tr_complete_return_to_safe_state'
        else:
            # task aborted during blockage
            rospy.loginfo("[StateMachine] Executing state: TaskCompletionRaport INPUT: " +
                          str(ud.auto_task_blocked_input))
            if ud.auto_task_blocked_input == 'user_abort':
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_transition = 'tr_complete_return_to_safe_state'
            if ud.auto_task_blocked_input == 'user_suspend':
                ud.task_completion_man_out = 'application_input_received'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_transition = 'tr_suspended_return_to_manual'
        rospy.loginfo("[StateMachine] Leaving state: TaskCompletionRaport TRANSITION: " +
                      str(output_transition))
        return output_transition
