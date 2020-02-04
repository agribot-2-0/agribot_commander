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
        smach.State.__init__(self, outcomes=['tr_auto_mode_failed', 'tr_auto_control_prepared', 'tr_in_progress'],
                                output_keys=['auto_control_transition_out'])
        self.counter=0

    def execute(self, userdata):
        string_msg="Executing state AutomaticControlTransition "
        output_string=''
        #Prepare auto control succesful or not
        #
        #
        rospy.sleep(0.010)
       
        if self.counter < 20:
            rospy.loginfo(string_msg)
            userdata.auto_control_transition_out='auto_control_prepared'
            output_string='tr_auto_control_prepared'
        else:
            self.counter= self.counter+1
            rospy.loginfo(string_msg)
            output_string='tr_in_progress'

        if self.counter==30:
            self.counter=0
            output_string='tr_auto_control_prepared'

        return output_string
        
