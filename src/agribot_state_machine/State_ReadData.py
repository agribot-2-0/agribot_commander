#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
import string
from std_msgs.msg import Int16
import time
import threading

# define state Foo
class ReadData(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_send_data_def_event', 'tr_auto_ctrl_command', 'tr_rec_only_move_commands'],
                                    output_keys=['read_data_command_out'])
        self.mutex = threading.Lock()
        self.command_received = 0
        self.subscriber = rospy.Subscriber('/hal_mobile', Int16, self.callback)

    def callback(self, data):
        self.mutex.acquire()
        rospy.loginfo('Psikus')
        if data.data == 1:
            self.command_received = 1
        else:
            self.command_received = 0
        self.mutex.release()

    def execute(self, userdata):
        string_msg="Executing state ReadData "+str(self.command_received)
        #read and interpret received data 
        #
        #
        #
        output_string=''
        rospy.sleep(0.010)
        self.mutex.acquire()
        
        if self.command_received==0:
            userdata.read_data_command_out='send_data_def_event'
            output_string='tr_send_data_def_event'

        if self.command_received==1:
            userdata.read_data_command_out='aut_ctrl_command'
            command_received=0
            output_string='tr_auto_ctrl_command'

        if self.command_received==2:
            userdata.read_data_command_out='rec_only_move_commands'
            command_received=0
            output_string='tr_rec_only_move_commands'

        rospy.loginfo(string_msg)
        self.mutex.release()
        return output_string
       