#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


class IsTaskChosen(smach.State):
     # pylint: disable=too-few-public-methods
    """This state decides if we continue the partially executed task or we start a new one"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_is_active', 'tr_no_active_task'])

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument, no-self-use
        """Returns the proper transition based on the system configuration
        (the task is already chosen and we continue the realisation or it is a new task)"""
        output_string = ''
        #Check if task name has been chosen
        #
        #
        task_name = rospy.get_param('/agribot/current_task_name')
        if task_name == '':
            string_msg = "[State Machine] Executing state IsTaskChosen "
            rospy.loginfo(string_msg)
            output_string = 'tr_no_active_task'
        else:
            string_msg = "[State Machine] Executing state IsTaskChosen " + \
                        "tr_task_is_active active_path_name: " + \
                         str(task_name)
            rospy.loginfo(string_msg)
            output_string = 'tr_task_is_active'
        return output_string
