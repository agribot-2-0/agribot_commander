#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class MobileAppConnectionRoutine(smach.State):
    """This state is used to check the connection state, if we doesnt received hearbeat error etc"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_unstable',
                                             'tr_auto_ctrl_command',
                                             'tr_rec_only_move_commands'],
                             output_keys=['connection_check_out',
                                          'read_data_command_out'])
        self.mutex = threading.Lock()
        self.connection_failed_received = False
        self.command_received_only_move = False
        self.command_received_configuration = False
        self.command_received_autonomous = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_app_communication)

    def callback_app_communication(self, data):
        self.mutex.acquire()
        if rospy.has_param('/agribot/robot_current_state'):
            current_state = rospy.get_param('/agribot/robot_current_state')  
            #Check for error frames
            if data.can_id == 200:
                #heartbeat failed
                if ord(data.can_byte_array[0]) == 0x00:
                    if ord(data.can_byte_array[1]) == 0x0F:
                        rospy.loginfo('Received  failed connection frame')
                        self.connection_failed_received = True
            if current_state == 'manual_control_state':
                if data.can_id == 53:
                    self.command_received_only_move = True
                elif data.can_id == 31:
                    rospy.loginfo('Received autonomous control command')
                    self.command_received_autonomous = True
                elif data.can_id == 150:
                    rospy.loginfo('Received configuration command')
                    self.command_received_configuration = True
        self.mutex.release()

    def execute(self, ud):
        """Check the connection state, if we doesnt received hearbeat error etc, 
        otherwise return to Save-State state"""
        output_string=''
        rospy.loginfo("Executing MobileAppConnectionRoutine")
        rate = rospy.Rate(100) # ROS Rate at 100Hz
        
        while not rospy.is_shutdown():   
            self.mutex.acquire()
            if self.connection_failed_received:
                self.connection_failed_received = False
                ud.connection_check_out = 'failed_connection'
                output_string = 'tr_connection_is_unstable'
                self.mutex.release()
                break
            elif self.command_received_autonomous:
                self.command_received_autonomous = False
                ud.read_data_command_out = 'aut_ctrl_command'
                output_string = 'tr_auto_ctrl_command'
                self.mutex.release()
                break
            elif self.command_received_only_move:
                self.command_received_only_move = False
                ud.read_data_command_out = 'rec_only_move_commands'
                output_string = 'tr_rec_only_move_commands'
                self.mutex.release()
                break
            self.mutex.release()
            rate.sleep()
        return output_string
