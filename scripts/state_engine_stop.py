#!/usr/bin/env python
import rospy
import smach
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class EngineStop(smach.State):
     # pylint: disable=too-few-public-methods
    """This state decides if we the engine was stopped because of system failure
    or because the task was complited and we should go back to Standby"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_engine_stopped_normal',
                                       'tr_engine_stop_failure_reaction'],
                             input_keys=['engine_stop_reason'])

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument, no-self-use
        """Returns the proper transition based on the system configuration
        (the engine was stopped properly or it was a failure reaction)"""
        rospy.loginfo('[State Machine] Executing state EngineStop')
        output_transition = ''
        #Send engine stop frames
        #when engine is still, return the outcome
        #normal work, no system failure
        rospy.sleep(0.010)
        rospy.loginfo("[State Machine] Executing state: EngineStop INPUT: " +
                      str(ud.engine_stop_reason))
        if ud.engine_stop_reason == 'Normal':
            output_transition = 'tr_engine_stopped_normal'
        else:
            output_transition = 'tr_engine_stop_failure_reaction'
        rospy.loginfo("[State Machine] Leaving state: EngineStop TRANSITION: " +
                      str(output_transition))
        return output_transition
