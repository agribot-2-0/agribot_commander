#!/usr/bin/env python
from __future__ import print_function
import threading
import rospy
import rospkg
import smach
import roslib
from std_srvs.srv import Empty
from std_msgs.msg import String
from industrial_msgs.msg import RobotMode
from agribot_msgs.msg import AgribotMobileAppFrame
from agribot_msgs.msg import AgribotAppErrorFlag
from agribot_msgs.msg import AgribotObstacleDetection
from config_reader import ConfigReader
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class MobileAppConnectionRoutine(smach.State):
    # pylint: disable=too-many-instance-attributes
    """This state is used to check the connection state, if we doesnt received hearbeat error etc"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_unstable',
                                             'tr_auto_ctrl_command'],
                             output_keys=['connection_check_out',
                                          'read_data_command_out'])
        self.mutex = threading.Lock()
        self.connection_failed_received = False
        self.command_received_only_move = False
        self.command_received_configuration = False
        self.command_received_autonomous = False
        self.obstacle_in_move_direction = False
        self.choosed_task_name = ''
        self.state_msg = String()
        rospack = rospkg.RosPack()
        path_to_conf = rospack.get_path('agribot_map') + "/tasks/"
        self.config_reader = ConfigReader(path_to_conf)
        self.subscriber_choose_task = rospy.Subscriber('/agribot/state_machine/task',
                                                       String,
                                                       self.callback_choose_task)
        self.subscriber_app_frames = rospy.Subscriber('/agribot/mobile_app_communication',
                                                      AgribotMobileAppFrame,
                                                      self.callback_app_frames_communication)
        self.subscriber_robot_mode = rospy.Subscriber('/agribot/state_machine/mode',
                                                      RobotMode,
                                                      self.callback_app_mode_communication)
        self.subscriber_errors = rospy.Subscriber('/agribot/state_machine/error',
                                                  AgribotAppErrorFlag,
                                                  self.callback_app_error_communication)
        self.pub_state = rospy.Publisher('/agribot/state_machine/state',
                                         String,
                                         queue_size=10)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    AgribotObstacleDetection,
                                                    self.callback_obstacle_detected)

    def callback_choose_task(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'manual_control_state':
                self.choosed_task_name = data.data
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'manual_control_state':
                self.obstacle_in_move_direction = \
                bool(data.status != AgribotObstacleDetection.NO_OBSTACLE)
        self.mutex.release()

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
            if (data.error_flag == AgribotAppErrorFlag.APP_PROTOCOL_ERROR or\
                data.error_flag == AgribotAppErrorFlag.APP_MANUAL_ERROR) and\
                current_state == 'manual_control_state':
                #heartbeat failed or manual steering error
                rospy.loginfo('[State Machine] Received  failed connection frame')
                self.connection_failed_received = True
        self.mutex.release()

    def execute(self, ud): # pylint: disable=invalid-name
        """Check the connection state, if we doesnt received hearbeat error etc,
        otherwise return to Save-State state"""
        output_transition = ''
        self.state_msg = "MobileAppConnectionRoutine"
        self.pub_state.publish(self.state_msg)
        rospy.loginfo("[State Machine] Executing state: MobileAppConnectionRoutine INPUT: none")
        rate = rospy.Rate(20) # ROS Rate at 20 Hz
        while not rospy.is_shutdown():
            self.mutex.acquire()
            if self.connection_failed_received:
                self.connection_failed_received = False
                ud.connection_check_out = 'failed_connection'
                output_transition = 'tr_connection_is_unstable'
                self.mutex.release()
                break
            elif self.command_received_autonomous:
                self.command_received_autonomous = False
                ud.read_data_command_out = 'aut_ctrl_command'
                output_transition = 'tr_auto_ctrl_command'
                self.mutex.release()
                break
            elif self.command_received_only_move:
                self.command_received_only_move = False
                # check for the blocked path
                if self.obstacle_in_move_direction:
                    # change the  cmd vel arbiter to safe state
                    rospy.wait_for_service('/cmd_vel_arbiter_node/switch_to_navigation')
                    switch_to_navigation =\
                        rospy.ServiceProxy('/cmd_vel_arbiter_node/switch_to_navigation',
                                           Empty)
                    try:
                        switch_to_navigation()
                    except rospy.ServiceException as exc:
                        print("Service error: {}".format(exc))
                else:
                    # change the cmd vel arbiter to manual
                    rospy.wait_for_service('/cmd_vel_arbiter_node/switch_to_teleop')
                    switch_to_teleop =\
                        rospy.ServiceProxy('/cmd_vel_arbiter_node/switch_to_teleop',
                                           Empty)
                    try:
                        switch_to_teleop()
                    except rospy.ServiceException as exc:
                        print("Service did not process request: {}".format(exc))
            elif self.choosed_task_name != '':
                path_name = self.config_reader.check_config_file(self.choosed_task_name)
                if path_name != '':
                    rospy.set_param('/agribot/chosen_task_name',
                                    path_name)
                self.choosed_task_name = ''
            self.mutex.release()
            try:
                rate.sleep()
            except rospy.ROSInterruptException as exc_mob:
                print("[State Machine]: {}".format(exc_mob))
        rospy.loginfo("[State Machine] Leaving state: MobileAppConnectionRoutine TRANSITION: " +
                      str(output_transition))
        return output_transition
