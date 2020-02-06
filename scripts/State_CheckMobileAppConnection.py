#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class CheckMobileAppConnection(smach.State):
    """This state is used to check the connection state, if we doesnt received hearbeat error etc"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_stable'])

    def execute(self, ud):
        """Check the connection state, if we doesnt received hearbeat error etc, 
        otherwise return to Save-State state"""
        rospy.sleep(0.010)
        rospy.loginfo('Executing state CheckMobileAppConnection')
        return 'tr_connection_is_stable'
