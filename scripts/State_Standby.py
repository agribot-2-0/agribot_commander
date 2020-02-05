#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string
from std_msgs.msg import Int16
from agribot_commander.msg import mobile_app_frame
import time
import threading

# define state Foo
class Standby(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_app_input_received_standby', 'tr_default_on_standby'],
                                    output_keys=['safe_state_reason_out'])
        self.mutex = threading.Lock()
        self.connection_received = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', mobile_app_frame, self.callbackAppCommunication)

    def callbackAppCommunication(self, data):
        self.mutex.acquire()
        if data.can_id == 10:
            self.connection_received = True
        self.mutex.release()

    def execute(self, userdata):
        output_string=''
        self.mutex.acquire()
        if self.connection_received:
                #ok we received 2
                self.connection_received = False
                userdata.safe_state_reason_out='application_input_received'
                output_string='tr_app_input_received_standby'
        else:
            output_string='tr_default_on_standby'

        self.mutex.release()   
        return output_string
