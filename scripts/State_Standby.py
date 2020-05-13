#!/usr/bin/env python
import threading
import rospy
import rospkg
import roslib
import smach
from std_msgs.msg import String
from industrial_msgs.msg import RobotMode
from config_reader import ConfigReader
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class Standby(smach.State):
    # pylint: disable=too-few-public-methods, too-many-instance-attributes
    """This is a default robot state in which it awaits the input from application"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_app_input_received_standby',
                                       'tr_autonomous_config_standby'],
                             output_keys=['safe_state_reason_man_out',
                                          'safe_state_reason_auto_out'])
        self.mutex = threading.Lock()
        self.connection_received = False
        self.last_task_name = ''
        self.choosed_task_name = ''
        self.standby_com = True
        rospack = rospkg.RosPack()
        path_to_conf = rospack.get_path('agribot_map') + "/tasks/"
        self.config_reader = ConfigReader(path_to_conf)
        self.subscriber = rospy.Subscriber('/agribot/state_machine/mode',
                                           RobotMode,
                                           self.callback_app_communication)
        self.subscriber_choose_task = rospy.Subscriber('/agribot/state_machine/task',
                                                       String,
                                                       self.callback_choose_task)

    def callback_choose_task(self, data):
        self.mutex.acquire()
        self.choosed_task_name = data.data
        self.mutex.release()

    def callback_app_communication(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'safe_state':
            if data.val == RobotMode.MANUAL:
                self.connection_received = True
        self.mutex.release()

    def check_config_file(self):
        if self.choosed_task_name != '':
            if self.config_reader.set_task_name(self.choosed_task_name):
                if self.config_reader.get_task_conf_from_yaml():
                    self.last_task_name = self.choosed_task_name
                    path_name = self.config_reader.return_tasks_path_name()
                    rospy.set_param('/agribot/chosen_task_name',
                                    path_name)
                    rospy.loginfo("[State Machine] Loaded task configuration: " + \
                                  "chosen_path_name: '" + \
                                  path_name + "'")
                    config_present = True
                else:
                    rospy.loginfo("[State Machine] ERROR: Task configuration file '" + \
                                  self.choosed_task_name + ".yaml' is faulty")
                    self.choosed_task_name = ''
                    config_present = False
            else:
                rospy.loginfo("[State Machine] ERROR: Task configuration file '" + \
                              self.choosed_task_name + ".yaml' can't be opened or doesn't exist")
                self.choosed_task_name = ''
                config_present = False
        else:
            self.choosed_task_name = ''
            config_present = False
        return config_present

    def execute(self, ud): # pylint: disable=invalid-name
    # pylint: disable=duplicate-code
        """Awaiting the application input"""
        if self.standby_com:
            string_msg = "[State Machine] Executing state Standby "
            rospy.loginfo(string_msg)
            self.standby_com = False
        output_string = ''
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        while not rospy.is_shutdown():
        # pylint: disable=duplicate-code
            self.mutex.acquire()
            if self.connection_received:
                self.connection_received = False
                self.standby_com = True
                ud.safe_state_reason_man_out = 'application_input_received'
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state',
                                'manual_control_state')
                output_string = 'tr_app_input_received_standby'
                self.mutex.release()
                break
            elif self.check_config_file():
                self.standby_com = True
                self.choosed_task_name = ''
                ud.safe_state_reason_man_out = ''
                ud.safe_state_reason_auto_out = 'autonomous_config_present'
                rospy.set_param('/agribot/robot_current_state',
                                'auto_control_state')
                output_string = 'tr_autonomous_config_standby'
                self.mutex.release()
                break
            else:
                ud.safe_state_reason_man_out = ' '
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state',
                                'safe_state')
            self.mutex.release()
            try:
                rate.sleep()
            except rospy.ROSInterruptException as exc_standby:
                # pylint: disable=superfluous-parens
                print("[State Machine]: " + str(exc_standby))
        return output_string
