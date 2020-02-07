#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class LogsAndProtocolData(smach.State):
    """This state is used to create logs of events, system state and received protocol data"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_logs_and_protocol_data_sent'])

    def execute(self, ud):
        """Create logs of events, system state and of received protocol data/commands"""
        #Send protocol data to mobile app and save system logs
        #
        #
        rospy.sleep(0.010)
        string_msg = "Executing state LogsAndProtocolData "
        rospy.loginfo(string_msg)
        return 'tr_logs_and_protocol_data_sent'
