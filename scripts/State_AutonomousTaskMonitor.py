#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
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
        self.counter = 0

    def callback_appcommunication(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')  
            if current_state == 'auto_control_state':
                self.counter = self.counter + 1
            else: 
                self.counter = 0   
        self.mutex.release()

    def execute(self, ud):
        """Monitore the task progression, safety,
         internal health of the system, weather conditions, create logs"""
        rospy.sleep(0.100)
        dummy = False
        while(self.counter == 10000000):
            dummy = True
        return 'tr_succes'
