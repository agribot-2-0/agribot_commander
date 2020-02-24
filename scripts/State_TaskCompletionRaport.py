#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class TaskCompletionRaport(smach.State):
    """State is used to create a summary report from from the Agribots work"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_complete'],
                             output_keys=['task_completion_output'])

    def execute(self, ud):
        """Create a report from the task realisation"""
        #Write raport data and go back to Standby
        #
        #
        string_msg = "Executing state TaskCompletionRaport "
        ud.task_completion_output = 'return_to_safe_state'
        rospy.loginfo(string_msg)
        rospy.set_param('/agribot/robot_current_state', 'safe_state')
        return 'tr_complete'
