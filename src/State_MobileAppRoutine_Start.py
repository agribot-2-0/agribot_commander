#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class MobileAppRoutineStart(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['task_config_error', 'app_input_received'],
                            input_keys=['mobile_app_routine_reason_in'])

    def execute(self, userdata):
        #Send data to mobile app
        #
        #
        string_msg="Executing state MobileAppRoutineStart with input "+userdata.mobile_app_routine_reason_in
        if userdata.mobile_app_routine_reason_in=='app_input_received':
            #string_msg="Executing state MobileAppRoutineStart "
            rospy.loginfo(string_msg)
            return 'app_input_received'
        else:
            rospy.loginfo(string_msg)
            return 'task_config_error'

