#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class Standby(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['app_input_received_standby', 'default_on_standby'])
        self.counter = 0
        self.limit = rospy.get_param("~standby_timeout", 10)

    def execute(self, userdata):
        if self.counter < self.limit:
            self.counter += 1
            if self.counter%1000 == 0:
                string_msg="Executing state Standby waiting "+str(self.counter)
                rospy.loginfo(string_msg)
            return 'default_on_standby'
        else:
            self.counter=0
            return 'app_input_received_standby'
