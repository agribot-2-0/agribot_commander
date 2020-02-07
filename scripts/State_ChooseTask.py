#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class ChooseTask(smach.State):
    """State used to search the tasks database to pick the task for realisation"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_is_active', 'tr_task_conf_error'],
                             output_keys=['choose_task_output'])
        self.data_name_dummy = 'd'

    def execute(self, ud):
        """Pick and return the proper transition. Choose the proper the task name picked
        by the user or if it cant be found return to manual or safe state"""
        output_string = ''
        #Check if task name is ok and if the task itself is proper
        #
        #
        if len(self.data_name_dummy) == '':
            string_msg = "Executing state ChooseTask "
            rospy.loginfo(string_msg)
            ud.choose_task_output = 'task_is_active'
            output_string = 'tr_task_is_active'
        else:
            string_msg = "Executing state ChooseTask "
            rospy.loginfo(string_msg)
            ud.choose_task_output = 'task_config_error'
            output_string = 'tr_task_conf_error'
        return output_string
