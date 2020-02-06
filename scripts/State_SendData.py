#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


class SendData(smach.State):
    """This state is used to send data to mobile app"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_data_sent'])

    def execute(self, ud):
        """Send appriopriate data to mobile app"""
        #Send data to mobile app
        #
        #
        rospy.sleep(0.010)
        string_msg = "Executing state SendData "
        rospy.loginfo(string_msg)
        return 'tr_data_sent'
