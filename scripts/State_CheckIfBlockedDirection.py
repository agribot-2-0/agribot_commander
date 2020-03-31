#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class CheckIfBlockedDirection(smach.State):
    # pylint: disable=too-few-public-methods
    """In this state, we check if the direction chosen by user is not blocked by obstacles."""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_no_obstacles'])

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument, no-self-use
        """Based on the sensor readings, an blocked or not blocked transition is returned"""
        #Check if direction is blocked
        #
        #
        string_msg = "[State Machine] Executing state CheckIfBlockedDirection "
        rospy.loginfo(string_msg)
        return 'tr_no_obstacles'
