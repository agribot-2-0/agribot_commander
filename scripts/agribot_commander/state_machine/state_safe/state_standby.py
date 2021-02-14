#!/usr/bin/env python
import threading
import os
import rospy
import roslib
import smach
from std_msgs.msg import String
from industrial_msgs.msg import RobotMode
from agribot_common.agribot_parameters import get_parameter
from agribot_commander.state_machine.config_reader import ConfigReader
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

LOG_TAG = "StateMachine"


class Standby(smach.State):
    # pylint: disable=too-few-public-methods, too-many-instance-attributes
    """This is a default robot state in which it awaits the input from application"""
    def __init__(self):
        smach.State.__init__(
            self,
            outcomes=['tr_app_input_received_standby', 'tr_autonomous_config_standby'],
            output_keys=['safe_state_reason_man_out', 'safe_state_reason_auto_out'])
        self.mutex = threading.Lock()
        self.connection_received = False
        self.last_task_name = ''
        self.choosed_task_name = ''
        self.state_msg = String()
        self.standby_com = True
        try:
            path_to_conf = os.path.join(os.environ.get('AGRIBOT_HOME'),
                                        get_parameter('/agribot_map/tasks_path'))
        except ValueError as exception_msg:
            rospy.logerr("[{}] {}".format(LOG_TAG, exception_msg))
            return

        self.config_reader = ConfigReader(path_to_conf)
        self.subscriber = rospy.Subscriber('/agribot/state_machine/mode', RobotMode,
                                           self.callback_app_communication)
        self.subscriber_choose_task = rospy.Subscriber('/agribot/state_machine/task', String,
                                                       self.callback_choose_task)
        self.pub_state = rospy.Publisher('/agribot/state_machine/state', String, queue_size=10)

    def callback_choose_task(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'safe_state':
                self.choosed_task_name = data.data
        self.mutex.release()

    def callback_app_communication(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'safe_state':
            if data.val == RobotMode.MANUAL:
                self.connection_received = True
        self.mutex.release()

    def execute(self, ud):  # pylint: disable=invalid-name
        # pylint: disable=duplicate-code
        """Awaiting the application input"""
        rospy.loginfo("[StateMachine] Executing state: Standby INPUT: none")
        self.state_msg = "Standby"
        self.pub_state.publish(self.state_msg)
        output_transition = ''
        rate = rospy.Rate(100)  # ROS Rate at 100Hz
        while not rospy.is_shutdown():
            # pylint: disable=duplicate-code
            self.mutex.acquire()
            if self.connection_received:
                self.connection_received = False
                ud.safe_state_reason_man_out = 'application_input_received'
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_transition = 'tr_app_input_received_standby'
                self.mutex.release()
                break
            elif self.choosed_task_name != '':
                path_name = self.config_reader.check_config_file(self.choosed_task_name)
                self.choosed_task_name = ''
                if path_name != '':
                    ud.safe_state_reason_man_out = ''
                    ud.safe_state_reason_auto_out = 'autonomous_config_present'
                    rospy.set_param('/agribot/robot_current_state', 'auto_control_state')
                    rospy.set_param('/agribot/chosen_task_name', path_name)
                    output_transition = 'tr_autonomous_config_standby'
                    self.mutex.release()
                    break
            else:
                ud.safe_state_reason_man_out = ' '
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
            self.mutex.release()
            try:
                rate.sleep()
            except rospy.ROSInterruptException:
                rospy.logerr("[StateMachine]: rospy shutdown exception")
        rospy.loginfo("[StateMachine] Leaving state: Standby TRANSITION: " + str(output_transition))
        return output_transition
