#!/usr/bin/env python
import rospy
import roslib
import smach
from std_msgs.msg import String
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class AutonomousControlTransition(smach.State):
    # pylint: disable=too-few-public-methods
    """This states is responsible for preparations to entry autonomous control and ensure
    the safe transition between manual and automatic control"""
    def __init__(self):
        smach.State.__init__(
            self,
            outcomes=['tr_auto_mode_failed', 'tr_auto_control_prepared', 'tr_in_progress'],
            output_keys=['auto_control_transition_out'])
        self.pub_state = rospy.Publisher('/agribot/state_machine/state', String, queue_size=10)
        self.state_msg = String()

    def execute(self, ud):  # pylint: disable=invalid-name
        """Checks if agribot is ready for the autonomous control, all needed hardware is inited,
        configured and apriopriate ROS nodes are functional"""
        output_transition = ''
        #Prepare auto control succesful or not
        # TO DO
        # HB check, catch TTControl answer ready to auto control
        rospy.loginfo("[StateMachine] Executing state: AutonomousControlTransition INPUT: none")
        self.state_msg = "AutonomousControlTransition"
        self.pub_state.publish(self.state_msg)
        try:
            rospy.sleep(0.010)
        except rospy.ROSInterruptException:
            rospy.logerr("[StateMachine]: rospy shutdown exception")
        ud.auto_control_transition_out = 'auto_control_prepared'
        rospy.set_param('/agribot/robot_current_state', 'auto_control_state')
        output_transition = 'tr_auto_control_prepared'
        rospy.loginfo("[StateMachine] Leaving state: AutonomousControlTransition TRANSITION: " +
                      str(output_transition))
        return output_transition
