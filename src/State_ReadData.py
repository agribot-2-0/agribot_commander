#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class ReadData(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['send_data_def_event', 'auto_ctrl_command', 'rec_only_move_commands'])
        self.data_dummy=0

    def execute(self, userdata):
        string_msg="Executing state ReadData "
        #read and interpret received data 
        #
        #
        #
        rospy.loginfo(string_msg)
        if self.data_dummy==0:
            return 'send_data_def_event'
        elif self.data_dummy==1:
            return 'aut_ctrl_command'
        elif self.data_dummy==2:
            return 'rec_only_move_commands'