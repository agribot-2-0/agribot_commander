#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class CreateOutputInfoLogs(smach.State):
     # pylint: disable=too-few-public-methods
    """This state create the sensory feedback and move commands logs"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_executed_outputs'])

    def execute(self, ud): # pylint: disable=invalid-name
        """Create logs of sensory feedback mesaures and move commands"""
        #Create output info logs
        #
        #
        string_msg = "[State Machine] Executing state CreateOutputInfoLogs "
        rospy.loginfo(string_msg)
        return 'tr_executed_outputs'
