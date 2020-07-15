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
        smach.State.__init__(self, outcomes=['tr_complete_return_to_safe_state',
                                             'tr_suspended_return_to_manual'],
                             output_keys=['task_completion_safe_out',
                                          'task_completion_man_out'],
                             input_keys=['auto_task_monitor_input',
                                         'auto_task_blocked_input'])
        self.pub_state = rospy.Publisher('/agribot/state_machine/state',
                                         String,
                                         queue_size=10)
        self.state_msg = String('')

    def execute(self, ud): # pylint: disable=invalid-name, no-self-use
        """Create a report from the task realisation"""
        #Write raport data and go back to Standby
        #
        #
        string_msg = "[State Machine] Executing state TaskCompletionRaport "
        rospy.loginfo(string_msg)
        self.state_msg = "TaskCompletionRaport"
        self.pub_state.publish(self.state_msg)
        current_state = rospy.get_param('/agribot/robot_current_state')
        output_str = ''
        if current_state == 'auto_control_state':
            if ud.auto_task_monitor_input == 'user_abort':
                string_msg = "[State Machine] Task aborted by the user"
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_str = 'tr_complete_return_to_safe_state'

            if ud.auto_task_monitor_input == 'user_suspend':
                string_msg = "[State Machine] Task suspended by the user for manual control"
                ud.task_completion_man_out = 'application_input_received'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_str = 'tr_suspended_return_to_manual'

            if ud.auto_task_monitor_input == 'succes':
                string_msg = "[State Machine] Task completed, all went good"
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_str = 'tr_complete_return_to_safe_state'
        else:
            # task aborted during blockage
            if ud.auto_task_blocked_input == 'user_abort':
                string_msg = "[State Machine] Task ended with human intervention"
                ud.task_completion_safe_out = 'return_to_safe_state'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
                # clear the current task
                rospy.set_param('/agribot/current_task_name', '')
                output_str = 'tr_complete_return_to_safe_state'
            if ud.auto_task_blocked_input == 'user_suspend':
                string_msg = "[State Machine] Task suspended by the user for manual control"
                ud.task_completion_man_out = 'application_input_received'
                # set current robot state
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_str = 'tr_suspended_return_to_manual'
        rospy.loginfo(string_msg)
        return output_str
