#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string

# define state Foo
class LogsAndProtocolData(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_logs_and_protocol_data_sent'])

    def execute(self, userdata):
        #Send protocol data to mobile app and save system logs
        #
        #
        rospy.sleep(0.010)
        string_msg="Executing state LogsAndProtocolData "
        rospy.loginfo(string_msg)
        return 'tr_logs_and_protocol_data_sent'
