#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class ChooseTask(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_is_active','tr_task_conf_error'],
                                    output_keys=['choose_task_output'])
        self.data_name_dummy='d'

    def execute(self, userdata):
        output_string=''
        #Check if task name is ok and if the task itself is proper
        #
        #
        if len(self.data_name_dummy)==0:
            string_msg="Executing state ChooseTask "
            rospy.loginfo(string_msg)
            userdata.choose_task_output='task_is_active'
            output_string='tr_task_is_active'
        else:
            string_msg="Executing state ChooseTask "
            rospy.loginfo(string_msg)
            userdata.choose_task_output='task_config_error'
            output_string='tr_task_conf_error'

        return output_string
