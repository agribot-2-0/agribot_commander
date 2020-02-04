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
        self.counter = 0
        self.mutex = threading.Lock()
        self.two_received = False
        self.limit = rospy.get_param("~standby_timeout", 10)
        self.subscriber = rospy.Subscriber('/test', Int16, self.callback)

    def callback(self, data):
        self.mutex.acquire()
        if data.data == 2:
            self.two_received = True
        self.mutex.release()

    def execute(self, userdata):
        output_string=''
        self.mutex.acquire()
        if self.two_received:
                #ok we received 2
                userdata.safe_state_reason_out='application_input_received'
                output_string='tr_app_input_received_standby'
        
        elif self.counter < self.limit:
            self.counter += 1
            if self.counter%1000 == 0:
                string_msg="Executing state Standby waiting "+str(self.counter)
                rospy.loginfo(string_msg)
            rospy.sleep(0.010)
            output_string='tr_default_on_standby'
        else:
            self.counter=0
            output_string='tr_default_on_standby'

        self.mutex.release()   
        return output_string
