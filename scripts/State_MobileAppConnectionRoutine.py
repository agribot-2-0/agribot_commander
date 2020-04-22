#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
from industrial_msgs.msg import RobotMode
from std_msgs.msg import Int32
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class MobileAppConnectionRoutine(smach.State):
    # pylint: disable=too-many-instance-attributes
    """This state is used to check the connection state, if we doesnt received hearbeat error etc"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_unstable',
                                             'tr_auto_ctrl_command',
                                             'tr_rec_only_move_commands'],
                             output_keys=['connection_check_out',
                                          'read_data_command_out'])
        self.mutex = threading.Lock()
        self.connection_failed_received = False
        self.command_received_only_move = False
        self.command_received_configuration = False
        self.command_received_autonomous = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication',
                                           AgribotMobileAppFrame,
                                           self.callback_app_frames_communication)
        self.subscriber_robot_mode = rospy.Subscriber('/agribot/state_machine/mode',
                                                      RobotMode,
                                                      self.callback_app_mode_communication)
        self.subscriber_errors = rospy.Subscriber('/agribot/state_machine/error',
                                                  Int32,
                                                  self.callback_app_error_communication)

    def callback_app_frames_communication(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'manual_control_state':
                if data.can_id == 53:
                    self.command_received_only_move = True
                elif data.can_id == 150:
                    rospy.loginfo('[State Machine] Received configuration command')
                    self.command_received_configuration = True
        self.mutex.release()

    def callback_app_mode_communication(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'manual_control_state':
                if data.val == RobotMode.AUTO:
                    rospy.loginfo('[State Machine] Received autonomous control command')
                    self.command_received_autonomous = True
        self.mutex.release()

    def callback_app_error_communication(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            #Check for error frames
            if (data.data == 1 or data.data == 2) and current_state == 'manual_control_state':
                #heartbeat failed or manual steering error
                rospy.loginfo('[State Machine] Received  failed connection frame')
                self.connection_failed_received = True
        self.mutex.release()

    def execute(self, ud): # pylint: disable=invalid-name
        """Check the connection state, if we doesnt received hearbeat error etc,
        otherwise return to Save-State state"""
        output_string = ''
        rospy.loginfo("[State Machine] Executing MobileAppConnectionRoutine")
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        while not rospy.is_shutdown():
            self.mutex.acquire()
            if self.connection_failed_received:
                self.connection_failed_received = False
                ud.connection_check_out = 'failed_connection'
                output_string = 'tr_connection_is_unstable'
                self.mutex.release()
                break
            elif self.command_received_autonomous:
                self.command_received_autonomous = False
                ud.read_data_command_out = 'aut_ctrl_command'
                output_string = 'tr_auto_ctrl_command'
                self.mutex.release()
                break
            elif self.command_received_only_move:
                self.command_received_only_move = False
                ud.read_data_command_out = 'rec_only_move_commands'
                output_string = 'tr_rec_only_move_commands'
                self.mutex.release()
                break
            self.mutex.release()
            try:
                rate.sleep()
            except rospy.ROSInterruptException as exc_mob:
                # pylint: disable=superfluous-parens
                print("[State Machine]: " + str(exc_mob))
        return output_string
