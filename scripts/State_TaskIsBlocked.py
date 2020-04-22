#!/usr/bin/env python
import threading
import rospy
import roslib
import smach
from industrial_msgs.msg import RobotMode
from std_msgs.msg import UInt32
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class TaskIsBlocked(smach.State):
    # pylint: disable=too-few-public-methods
    """This is a danger responding robot state, the danger can be a blocked
    path or the enviroment is no longer favorable for the task"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_human_intervention',
                                       'tr_continue_task'])
        self.mutex = threading.Lock()
        self.intervention_received = False
        self.blocked_path = False
        self.app_subscriber = rospy.Subscriber('/agribot/state_machine/mode',
                                               RobotMode,
                                               self.callback_app_communication)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    UInt32,
                                                    self.callback_obstacle_detected)

    def callback_app_communication(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state':
            if data.val == RobotMode.MANUAL:
                self.intervention_received = True
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        self.blocked_path = bool(data.data != 0)
        self.mutex.release()

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument
    # pylint: disable=duplicate-code
        """Solving the problem by waiting, loading recovery task or receiving human intervention"""
        string_msg = "[State Machine] Executing state TaskIsBlocked "
        rospy.loginfo(string_msg)
        output_string = ''
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        while not rospy.is_shutdown():
            self.mutex.acquire()
            if self.intervention_received:
                self.intervention_received = False
                rospy.set_param('/agribot/robot_current_state',
                                'manual_control_state')
                output_string = 'tr_human_intervention'
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
                # pylint: disable=superfluous-parens
                print("[State Machine]: " + str(exc))
            # pylint: enable=duplicate-code
        return output_string
