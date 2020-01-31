#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class AutonomousControlTransition(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['auto_mode_failed', 'auto_control_prepared'],
                                output_keys=['auto_control_transition_out'])
        self.counter=0

    def execute(self, userdata):
        string_msg="Executing state AutomaticControlTransition "
        #Prepare auto control succesful or not
        #
        #

        if self.counter < 20:
            rospy.loginfo(string_msg)
            userdata.auto_control_transition_out='auto_control_prepared'
            return 'auto_control_prepared'
        else:
            self.counter=0
            rospy.loginfo(string_msg)
            return 'auto_mode_failed'
