#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys

# define state Foo
class EngineStop(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_engine_stopped_normal', 'tr_engine_stop_failure_reaction'], 
                            input_keys=['engine_stop_reason'])

    def execute(self, userdata):
        rospy.loginfo('Executing state EngineStop')
        #Send engine stop frames
        #when engine is still, return the outcome
        #normal work, no system failure
        rospy.sleep(0.010)
        if userdata.engine_stop_reason == 'Normal':
            return 'tr_engine_stopped_normal'
        else:
            return 'tr_engine_stop_failure_reaction'
