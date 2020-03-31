#!/usr/bin/env python
import rospy
import roslib
import smach
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

# define state Foo
class AutonomousControlTransition(smach.State):
    # pylint: disable=too-few-public-methods
    """This states is responsible for preparations to entry autonomous control and ensure
    the safe transition between manual and automatic control"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_auto_mode_failed', 'tr_auto_control_prepared',
                                             'tr_in_progress'],
                             output_keys=['auto_control_transition_out'])
        self.counter = 0

    def execute(self, ud): # pylint: disable=invalid-name
        """Checks if agribot is ready for the autonomous control, all needed hardware is inited,
        configured and apriopriate ROS nodes are functional"""
        output_string = ''
        #Prepare auto control succesful or not
        #
        #
        rospy.sleep(0.010)
        if self.counter < 20:
            ud.auto_control_transition_out = 'auto_control_prepared'
            rospy.set_param('/agribot/robot_current_state', 'auto_control_state')
            output_string = 'tr_auto_control_prepared'
        else:
            self.counter = self.counter+1
            output_string = 'tr_in_progress'

        if self.counter == 30:
            self.counter = 0
            rospy.set_param('/agribot/robot_current_state', 'auto_control_state')
            output_string = 'tr_auto_control_prepared'

        return output_string
        