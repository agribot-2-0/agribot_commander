#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMoveBaseMonitorResult
from industrial_msgs.msg import RobotMode
from std_msgs.msg import Int32
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousTaskMonitor(smach.State):
    # pylint: disable=too-few-public-methods, too-many-instance-attributes
    """State is used to monitore the task progression, safety,
     internal health of the system, weather conditions, create logs"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_succes', 'tr_danger'],
                             output_keys=['auto_task_monitor_output'])
        self.mutex = threading.Lock()
        self.command_received_only_move = False
        self.subscriber = rospy.Subscriber('/agribot/state_machine/mode',
                                           RobotMode,
                                           self.callback_app_communication)
        self.sub_move_base_result = rospy.Subscriber('/agribot/move_base_monitor/result',
                                                     AgribotMoveBaseMonitorResult,
                                                     self.callback_move_base_result)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    Int32,
                                                    self.callback_obstacle_detected)
        self.pub_move_base_comm = rospy.Publisher('/agribot/move_base_monitor/command',
                                                  Int32,
                                                  queue_size=10)
        #self.move_base = actionlib.SimpleActionClient("/move_base", MoveBaseAction)
        self.move_base_command_msg = Int32(0)
        self.move_base_result = 10 # the 'no action' value
        self.move_base_current_waypoint = 0
        self.move_base_total_waypoints = 0
        self.received_manual_steering = False
        self.task_is_blocked = False

    #def callback_app_error_communication(self, data):
    #   self.mutex.acquire()
    #   if data.data != 0:
    #
    #   self.mutex.release()

    def callback_app_communication(self, data):
        ##TO DO
        ##The manual request abort task procedure is needed
        ##
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'auto_control_state':
                if data.val == RobotMode.MANUAL:
                    self.received_manual_steering = True
                else:
                    self.received_manual_steering = False
        self.mutex.release()

    def callback_move_base_result(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state') and \
           rospy.has_param('/agribot/current_task_name'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            current_task = rospy.get_param('/agribot/current_task_name')
            if current_state == 'auto_control_state' and \
               current_task != '':
                self.move_base_result = data.result
                self.move_base_current_waypoint = data.current_waypoint
                self.move_base_total_waypoints = data.total_waypoints
            else:
                self.move_base_result = 10
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state':
            self.task_is_blocked = bool(data.data != 0)
        self.mutex.release()

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rospy.sleep(0.100)
        string_msg = "[State Machine] Executing state AutonomousTaskMonitor "
        rospy.loginfo(string_msg)
        outcome_name = ''
        #trace the position, wait for safety events or mobile app interruption
        while True:
            self.mutex.acquire()
            # check if the task is aborted by manual steering
            if self.received_manual_steering:
                outcome_name = 'tr_succes'
                ud.auto_task_monitor_output = 'user_abort'
                #send abort current path to move_base_monitor
                self.received_manual_steering = False
                self.move_base_command_msg.data = 2
                self.pub_move_base_comm.publish(self.move_base_command_msg)
                self.mutex.release()
                break
            # check if the task is not blocked
            if self.task_is_blocked:
                rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                              because current task is blocked by obstacle")
                #send pause goal to move_base_monitor
                self.move_base_command_msg.data = 1
                self.pub_move_base_comm.publish(self.move_base_command_msg)

                outcome_name = 'tr_danger'
                self.mutex.release()
                break
            else:
                #send continue command to move_base_monitor
                self.move_base_command_msg.data = 0
                self.pub_move_base_comm.publish(self.move_base_command_msg)
            # if task is loaded
            if self.move_base_result != 10:
                 #if task ended succesfully
                if self.move_base_result == 100:
                    ud.auto_task_monitor_output = 'succes'
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
                #task pending for processing or is active
                if self.move_base_result == 0 or \
                   self.move_base_result == 1 or \
                   self.move_base_result == 6 or \
                   self.move_base_result == 7:
                    #goal is pending, active, or being prempted or recalled
                    outcome_name = ''
            self.mutex.release()
        return outcome_name
