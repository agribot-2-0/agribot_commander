#!/usr/bin/env python
from __future__ import print_function
import threading
import rospy
import roslib
import smach
from industrial_msgs.msg import RobotMode
from std_msgs.msg import String
from agribot_msgs.msg import AgribotObstacleDetection
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class TaskIsBlocked(smach.State):
    # pylint: disable=too-few-public-methods, too-many-instance-attributes
    """This is a danger responding robot state, the danger can be a blocked
    path or the enviroment is no longer favorable for the task"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_human_intervention',
                                       'tr_continue_task'],
                             output_keys=['auto_task_blocked_output'])
        self.mutex = threading.Lock()
        self.intervention_received = False
        self.user_abort = False
        self.blocked_path = False
        self.app_subscriber = rospy.Subscriber('/agribot/state_machine/mode',
                                               RobotMode,
                                               self.callback_app_communication)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    AgribotObstacleDetection,
                                                    self.callback_obstacle_detected)
        self.pub_state = rospy.Publisher('/agribot/state_machine/state',
                                         String,
                                         queue_size=10)
        self.state_msg = String()

    def callback_app_communication(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state':
            self.intervention_received = bool(data.val == RobotMode.MANUAL)
            self.user_abort = bool(data.val == RobotMode.UNKNOWN)
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        self.blocked_path = \
        bool(data.status == AgribotObstacleDetection.OBSTACLE_DETECTED)
        self.mutex.release()

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument
    # pylint: disable=duplicate-code
        """Solving the problem by waiting, loading recovery task or receiving human intervention"""
        string_msg = "[State Machine] Executing state TaskIsBlocked "
        rospy.loginfo(string_msg)
        output_string = ''
        self.state_msg = "TaskIsBlocked"
        self.pub_state.publish(self.state_msg)
        self.intervention_received = False
        self.user_abort = False
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        while not rospy.is_shutdown():
            self.mutex.acquire()
            if self.intervention_received:
                self.intervention_received = False
                rospy.set_param('/agribot/robot_current_state',
                                'manual_control_state')
                output_string = 'tr_human_intervention'
                ud.auto_task_blocked_output = 'user_suspend'
                self.mutex.release()
                break
            if self.user_abort:
                self.user_abort = False
                rospy.set_param('/agribot/robot_current_state',
                                'manual_control_state')
                output_string = 'tr_human_intervention'
                ud.auto_task_blocked_output = 'user_abort'
                self.mutex.release()
                break
            if not self.blocked_path:
                rospy.set_param('/agribot/robot_current_state',
                                'auto_control_state')
                output_string = 'tr_continue_task'
                string_msg = "[State Machine] Task is no longer blocked, leaving the blocked state"
                self.mutex.release()
                rospy.loginfo(string_msg)
                break
            self.mutex.release()
            # pylint: disable=duplicate-code
            try:
                rate.sleep()
            except rospy.ROSInterruptException as exc:
                print("[State Machine]: {}".format(exc))
            # pylint: enable=duplicate-code
        return output_string
