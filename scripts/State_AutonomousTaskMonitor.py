#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
from actionlib_msgs.msg import GoalStatusArray
from std_msgs.msg import UInt32
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousTaskMonitor(smach.State):
    # pylint: disable=too-few-public-methods
    """State is used to monitore the task progression, safety,
     internal health of the system, weather conditions, create logs"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_succes', 'tr_danger'])
        self.mutex = threading.Lock()
        self.command_received_only_move = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication',
                                           AgribotMobileAppFrame,
                                           self.callback_appcommunication)
        self.sub_goal_status = rospy.Subscriber('/move_base/status',
                                                GoalStatusArray,
                                                self.callback_move_base_status)
        self.obstacle_subscriber = rospy.Subscriber('/agribot/obstacle_detected',
                                                    UInt32,
                                                    self.callback_obstacle_detected)
        self.goal_status = 10 # the 'no action' value
        self.received_frame_id = 0
        self.task_is_blocked = False

    def callback_appcommunication(self, data):
        ##TO DO
        ##The manual request abort task procedure is needed
        ##
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            if current_state == 'auto_control_state':
                self.received_frame_id = data.can_id
        self.mutex.release()

    def callback_move_base_status(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state') and \
           rospy.has_param('/agribot/current_task_name'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            current_task = rospy.get_param('/agribot/current_task_name')
            if current_state == 'auto_control_state' and \
               current_task != '' and \
               data.status_list:
                self.goal_status = data.status_list[0].status
                #rospy.loginfo(self.goal_status)
            else:
                self.goal_status = 10
        self.mutex.release()

    def callback_obstacle_detected(self, data):
        self.mutex.acquire()
        current_state = rospy.get_param('/agribot/robot_current_state')
        if current_state == 'auto_control_state':
            if data.data != 0:
                self.task_is_blocked = True
            else:
                self.task_is_blocked = False
        self.mutex.release()       

    def execute(self, ud): # pylint: disable=invalid-name, unused-argument
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rospy.sleep(0.100)
        string_msg = "[State Machine] Executing state AutonomousTaskMonitor "
        rospy.loginfo(string_msg)
        outcome_name = ''
        #false_preemptive_trigerred = true
        while True:
            self.mutex.acquire()
            # check if the task is not blocked
            if self.task_is_blocked:
                rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                              because current task is blocked by obstacle")
                outcome_name = 'tr_danger'
                self.mutex.release()
                break
            # if task is loaded
            if self.goal_status != 10:
                 #if task ended succesfully
                if self.goal_status == 3:
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
                #task pending for processing or is active
                if self.goal_status == 0 or \
                   self.goal_status == 1 or \
                   self.goal_status == 6 or \
                   self.goal_status == 7:
                    #goal is pending, active, or being prempted or recalled
                    #trace the position, wait for safety events or mobile app interruption
                    outcome_name = ''
                #if (self.goal_status == 2):
                #    if not false_preemptive_trigerred:
                #        rospy.loginfo("Leaving AutonomousTaskMonitor state \
                #                      because current goal was PREEMPTED")
                #        outcome_name = 'tr_succes'
                #        self.mutex.release()
                #        break
                #    else:
                #        false_preemptive_trigerred = False
                if self.goal_status == 4:
                    rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                                  because current goal was ABORTED")
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
                if self.goal_status == 6:
                    rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                                  because current goal was REJECTED")
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
                if self.goal_status == 8:
                    rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                                  because current goal was RECALLED")
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
                if self.goal_status == 9:
                    rospy.loginfo("[State Machine] Leaving AutonomousTaskMonitor state \
                                  because current goal was LOST")
                    outcome_name = 'tr_succes'
                    self.mutex.release()
                    break
            self.mutex.release()
        return outcome_name
