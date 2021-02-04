#!/usr/bin/env python
import threading
import rospy
import smach
import roslib
from std_msgs.msg import String
from industrial_msgs.msg import RobotMode
from agribot_msgs.msg import AgribotMoveBaseCommand
from agribot_msgs.msg import AgribotObstacleDetection
from agribot_msgs.msg import AgribotMoveBaseMonitorResult
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


class AutonomousTaskMonitor(smach.State):
    # pylint: disable=too-few-public-methods, too-many-instance-attributes
    """State is used to monitore the task progression, safety,
     internal health of the system, weather conditions, create logs"""

    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_succes', 'tr_danger'],
                             output_keys=['auto_task_monitor_output'])
        self.mutex = threading.Lock()
        self.command_received_only_move = False
        self.subscriber = rospy.Subscriber('/agribot/state_machine/mode', RobotMode,
                                           self.callback_app_communication)
        self.sub_move_base_result = rospy.Subscriber('/agribot/move_base_monitor/result',
                                                     AgribotMoveBaseMonitorResult,
                                                     self.callback_move_base_result)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    AgribotObstacleDetection,
                                                    self.callback_obstacle_detected)
        self.pub_move_base_comm = rospy.Publisher('/agribot/move_base_monitor/command',
                                                  AgribotMoveBaseCommand,
                                                  queue_size=10)
        self.pub_state = rospy.Publisher('/agribot/state_machine/state', String, queue_size=10)
        self.state_msg = String()
        self.move_base_command_msg = AgribotMoveBaseCommand()
        self.move_base_result = AgribotMoveBaseMonitorResult.PAUSED
        self.move_base_current_waypoint = 0
        self.move_base_total_waypoints = 0
        self.received_manual_steering = False
        self.received_user_abort = False
        self.task_is_blocked = False
        self.warn_exception = False

    def callback_app_communication(self, data):
        ##TO DO
        ##The manual request abort task procedure is needed
        ##
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'auto_control_state':
                self.received_manual_steering = bool(data.val == RobotMode.MANUAL)
                self.received_user_abort = bool(data.val == RobotMode.UNKNOWN)
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
                self.move_base_result = AgribotMoveBaseMonitorResult.PAUSED
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state':
            self.task_is_blocked =\
            bool(data.status == AgribotObstacleDetection.OBSTACLE_DETECTED)
        self.mutex.release()

    def clear_manual_flag(self):
        self.mutex.acquire()
        if self.received_manual_steering or self.received_user_abort:
            self.received_manual_steering = False
            self.received_user_abort = False
            #send continue command to move_base_monitor
            self.move_base_command_msg.command = AgribotMoveBaseCommand.CONTINUE
            self.pub_move_base_comm.publish(self.move_base_command_msg)
        self.mutex.release()

    def pre_execute(self):
        self.move_base_result = AgribotMoveBaseMonitorResult.NOACTION
        self.warn_exception = True
        # Check if the task was suspended by the user during the task blockage
        # and should we continue the last path
        self.clear_manual_flag()

    def execute(self, ud):  # pylint: disable=invalid-name, unused-argument, too-many-statements
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rate = rospy.Rate(100)  # ROS Rate at 100Hz
        rospy.loginfo("[StateMachine] Executing state: AutonomousTaskMonitor INPUT: none")
        self.state_msg = "AutonomousTaskMonitor"
        self.pub_state.publish(self.state_msg)
        self.pre_execute()
        output_transition = ''
        #trace the position, wait for safety events or mobile app interruption
        while True:
            self.mutex.acquire()
            # check if the task is aborted by manual steering
            if self.received_manual_steering:
                output_transition = 'tr_succes'
                ud.auto_task_monitor_output = 'user_suspend'
                #send pause current path to move_base_monitor
                self.received_manual_steering = False
                self.move_base_command_msg.command = AgribotMoveBaseCommand.PAUSE
                self.pub_move_base_comm.publish(self.move_base_command_msg)
                self.mutex.release()
                break
            if self.received_user_abort:
                output_transition = 'tr_succes'
                ud.auto_task_monitor_output = 'user_abort'
                #send abort current path to move_base_monitor
                self.received_user_abort = False
                self.move_base_command_msg.command = AgribotMoveBaseCommand.ABORT
                self.pub_move_base_comm.publish(self.move_base_command_msg)
                self.mutex.release()
                break
            # check if the task is not blocked
            if self.task_is_blocked:
                #send pause goal to move_base_monitor
                self.move_base_command_msg.command = AgribotMoveBaseCommand.PAUSE
                self.pub_move_base_comm.publish(self.move_base_command_msg)

                output_transition = 'tr_danger'
                self.mutex.release()
                break
            else:
                #send continue command to move_base_monitor
                self.move_base_command_msg.command = AgribotMoveBaseCommand.CONTINUE
                self.pub_move_base_comm.publish(self.move_base_command_msg)
            # if task is loaded
            if self.move_base_result != AgribotMoveBaseMonitorResult.PAUSED:
                #if task ended succesfully
                if self.move_base_result == AgribotMoveBaseMonitorResult.PATH_COMPLETED:
                    ud.auto_task_monitor_output = 'succes'
                    output_transition = 'tr_succes'
                    self.mutex.release()
                    break
                #task pending for processing or is active
                if self.move_base_result == AgribotMoveBaseMonitorResult.NOACTION or \
                   self.move_base_result == AgribotMoveBaseMonitorResult.ACTIVE or \
                   self.move_base_result == AgribotMoveBaseMonitorResult.PREEMPTED or \
                   self.move_base_result == AgribotMoveBaseMonitorResult.RECALLED:
                    #goal is pending, active, or being prempted or recalled
                    output_transition = ''
            self.mutex.release()
            try:
                rate.sleep()
            except rospy.ROSInterruptException:
                if self.warn_exception:
                    rospy.logerr("[StateMachine]: rospy shutdown exception")
                    self.warn_exception = False
        rospy.loginfo("[StateMachine] Leaving state: AutonomousTaskMonitor TRANSITION: " +
                      output_transition)
        return output_transition
