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
                             outcomes=['tr_app_input_received_standby', 'tr_default_on_standby'],
                             output_keys=['safe_state_reason_out'])
        self.mutex = threading.Lock()
        self.connection_received = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_app_communication)

    def callback_app_communication(self, data):
        self.mutex.acquire()
        if data.can_id == 10:
            self.connection_received = True
        self.mutex.release()

    def execute(self, ud):
        """Awaiting the application input"""
        output_string = ''
        self.mutex.acquire()
        if self.connection_received:
            self.connection_received = False
            ud.safe_state_reason_out = 'application_input_received'
            output_string = 'tr_app_input_received_standby'
        else:
            output_string = 'tr_default_on_standby'
        self.mutex.release()
        return output_string
