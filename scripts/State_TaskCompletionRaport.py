#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class TaskCompletionRaport(smach.State):
    # pylint: disable=too-few-public-methods
    """State is used to create a summary report from from the Agribots work"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_complete'],
                             output_keys=['task_completion_output'],
                             input_keys=['auto_task_monitor_input'])

    def execute(self, ud): # pylint: disable=invalid-name, no-self-use
        """Create a report from the task realisation"""
        #Write raport data and go back to Standby
        #
        #
        string_msg = "[State Machine] Executing state TaskCompletionRaport "
        ud.task_completion_output = 'return_to_safe_state'
        rospy.loginfo(string_msg)
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state' and ud.auto_task_monitor_input != 'user_abort':
            string_msg = "[State Machine] Task completed, all went good"
        else:
            string_msg = "[State Machine] Task ended with human intervention"
        rospy.loginfo(string_msg)
        rospy.set_param('/agribot/robot_current_state', 'safe_state')
        rospy.set_param('/agribot/current_task_name', '')
        return 'tr_complete'
