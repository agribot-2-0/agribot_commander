#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


class IsTaskChosen(smach.State):
    """This state decides if we continue the partially executed task or we start a new one"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_is_active', 'tr_no_active_task'])
        self.data_name_dummy = ''

    def execute(self, ud):
        """Returns the proper transition based on the system configuration
        (the task is already chosen and we continue the realisation or it is a new task)"""
        output_string = ''
        #Check if task name has been chosen
        #
        #
        if len(self.data_name_dummy) == '':
            string_msg = "Executing state IsTaskChosen "
            rospy.loginfo(string_msg)
            output_string = 'tr_no_active_task'
        else:
            string_msg = "Executing state IsTaskChosen "
            rospy.loginfo(string_msg)
            output_string = 'tr_task_is_active'
        return output_string
