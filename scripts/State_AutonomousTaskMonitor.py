#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
from actionlib_msgs.msg import GoalStatusArray
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class AutonomousTaskMonitor(smach.State):
    """State is used to monitore the task progression, safety,
     internal health of the system, weather conditions, create logs"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_succes'])
        self.mutex = threading.Lock()
        self.command_received_only_move = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_appcommunication)
        self.sub_goal_status = rospy.Subscriber('/move_base/status', GoalStatusArray, self.callback_move_base_status)
        self.goal_status = 10 # the 'no action' value
        self.received_frame_id = 0

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
        if rospy.has_param('/agribot/robot_current_state') and rospy.has_param('/agribot/current_task_name'):
            current_state = rospy.get_param('/agribot/robot_current_state')
            current_task = rospy.get_param('/agribot/current_task_name')
            if current_state == 'auto_control_state' and len(current_task) != 0 and len(data.status_list) != 0:
                self.goal_status = data.status_list[0].status
                #rospy.loginfo(self.goal_status)
            else:
                self.goal_status = 10
        self.mutex.release()

    def execute(self, ud):
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rospy.sleep(0.100)
        string_msg = "Executing state AutonomousTaskMonitor "
        rospy.loginfo(string_msg)
        outcome_name = ''
        #false_preemptive_trigerred = true
        while(True):
            self.mutex.acquire()
            #if task is loaded
            if self.goal_status != 10:
                 #if task ended succesfully
                if self.goal_status == 3:
                    outcome_name = 'tr_succes'
                    break
                #task pending for processing or is active
                if (self.goal_status == 0) or (self.goal_status == 1) or (self.goal_status == 6) or (self.goal_status == 7):
                    #goal is pending, active, or being prempted or recalled
                    #trace the position, wait for safety events or mobile app interruption
                    outcome_name = ''
                #if (self.goal_status == 2):
                #    if not false_preemptive_trigerred:
                #        rospy.loginfo("Leaving AutonomousTaskMonitor state because current goal was PREEMPTED")
                #        outcome_name = 'tr_succes'
                #        break
                #    else:
                #        false_preemptive_trigerred = False
                if (self.goal_status == 4):
                    rospy.loginfo("Leaving AutonomousTaskMonitor state because current goal was ABORTED")
                    outcome_name = 'tr_succes'
                    break
                if (self.goal_status == 6):
                    rospy.loginfo("Leaving AutonomousTaskMonitor state because current goal was REJECTED")
                    outcome_name = 'tr_succes'
                    break
                if (self.goal_status == 8):
                    rospy.loginfo("Leaving AutonomousTaskMonitor state because current goal was RECALLED")
                    outcome_name = 'tr_succes'
                    break
                if (self.goal_status == 9):
                    rospy.loginfo("Leaving AutonomousTaskMonitor state because current goal was LOST")
                    outcome_name = 'tr_succes'
                    break
            self.mutex.release()
        return outcome_name
