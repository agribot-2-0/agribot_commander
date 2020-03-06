#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class Standby(smach.State):
    """This is a default robot state in which it awaits the input from application"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_app_input_received_standby'],
                             output_keys=['safe_state_reason_man_out', 'safe_state_reason_auto_out'])
        self.mutex = threading.Lock()
        self.connection_received = False
        self.standby_com = True
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_app_communication)

    def callback_app_communication(self, data):
        self.mutex.acquire()
        if data.can_id == 10:
            self.connection_received = True
        self.mutex.release()

    def execute(self, ud):
        """Awaiting the application input"""
        if self.standby_com:
            string_msg = "Executing state Standby "
            rospy.loginfo(string_msg)
            self.standby_com = False
        output_string = ''
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        
        while not rospy.is_shutdown():
            self.mutex.acquire()
            if self.connection_received:
                self.connection_received = False
                self.standby_com = True
                ud.safe_state_reason_man_out = 'application_input_received'
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                output_string = 'tr_app_input_received_standby'
                self.mutex.release()
                break
            else:
                ud.safe_state_reason_man_out = ' '
                ud.safe_state_reason_auto_out = ' '
                rospy.set_param('/agribot/robot_current_state', 'safe_state')
            self.mutex.release()
            rate.sleep()
        return output_string