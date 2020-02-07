#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

# define state Foo
class ReadData(smach.State):
    """This state is to pick the proper transition in reaction for the received data"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_send_data_def_event',
                                       'tr_auto_ctrl_command',
                                       'tr_rec_only_move_commands'],
                             output_keys=['read_data_command_out'])
        self.mutex = threading.Lock()
        self.command_received_only_move = False
        self.command_received_configuration = False
        self.command_received_autonomous = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_appcommunication)

    def callback_appcommunication(self, data):
        self.mutex.acquire()
        if data.can_id == 53:
            rospy.loginfo('Received only move commands')
            self.command_received_only_move = True
        elif data.can_id == 30:
            rospy.loginfo('Received autonomous control command')
            self.command_received_autonomous = True
        elif data.can_id == 150:
            rospy.loginfo('Received configuration command')
            self.command_received_configuration = True
        self.mutex.release()

    def execute(self, ud):
        """Pick and return the proper transition in reaction for the received data.
        Set the proper userdata (ud)"""
        string_msg = "Executing state ReadData "
        #read and interpret received data
        #
        #
        #
        output_string = ''
        rospy.sleep(0.010)
        self.mutex.acquire()
        if self.command_received_configuration:
            self.command_received_configuration = False
            ud.read_data_command_out = 'send_data_def_event'
            output_string = 'tr_send_data_def_event'
        elif self.command_received_autonomous:
            self.command_received_autonomous = False
            ud.read_data_command_out = 'aut_ctrl_command'
            output_string = 'tr_auto_ctrl_command'
        elif self.command_received_only_move:
            self.command_received_only_move = False
            ud.read_data_command_out = 'rec_only_move_commands'
            output_string = 'tr_rec_only_move_commands'
        else:
            self.command_received_configuration = False
            ud.read_data_command_out = 'send_data_def_event'
            output_string = 'tr_send_data_def_event'

        rospy.loginfo(string_msg)
        self.mutex.release()
        return output_string
       