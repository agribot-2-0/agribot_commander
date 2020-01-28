#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class IsTaskChosen(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['task_is_active','no_active_task'])
        self.data_name_dummy=''

    def execute(self, userdata):
        #Check if task name has been chosen
        #
        #
        if len(self.data_name_dummy)==0:
            string_msg="Executing state IsTaskChosen "
            rospy.loginfo(string_msg)
            return 'no_active_task'
        else:
            string_msg="Executing state IsTaskChosen "
            rospy.loginfo(string_msg)
            return 'task_is_active'
