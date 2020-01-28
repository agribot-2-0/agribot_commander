#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys

# define state Foo
class EngineStop(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['engine_stopped_normal', 'engine_stop_failure_reaction'], 
                            input_keys=['engine_stop_reason'])

    def execute(self, userdata):
        rospy.loginfo('Executing state EngineStop')
        #Send engine stop frames
        #when engine is still, return the outcome
        #normal work, no system failure
        if userdata.engine_stop_reason == 'Normal':
            return 'engine_stopped_normal'
        else:
            return 'engine_stop_failure_reaction'
