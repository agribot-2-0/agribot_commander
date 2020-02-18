#!/usr/bin/env python
import threading
import rospy
import smach
from agribot_msgs.msg import AgribotMobileAppFrame
import roslib
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')


# define state Foo
class CheckMobileAppConnection(smach.State):
    """This state is used to check the connection state, if we doesnt received hearbeat error etc"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_connection_is_stable', 'tr_connection_is_unstable'],
                             output_keys=['connection_check_out'])
        self.mutex = threading.Lock()
        self.connection_failed_received = False
        self.subscriber = rospy.Subscriber('/agribot/mobile_app_communication', AgribotMobileAppFrame, self.callback_app_communication)

    def callback_app_communication(self, data):
        self.mutex.acquire()
        #Check for error frames
        if data.can_id == 200:
            if data.can_byte_array[0] == 0x00:
                if data.can_byte_array[1] == 0x0F:
                    self.connection_failed_received = True
        self.mutex.release()

    def execute(self, ud):
        """Check the connection state, if we doesnt received hearbeat error etc, 
        otherwise return to Save-State state"""
        rospy.sleep(0.010)
        output_string=''
        if self.connection_failed_received:
            ud.connection_check_out = 'failed_connection'
            output_string = 'tr_connection_is_unstable'
        else:
            output_string = 'tr_connection_is_stable'
        return output_string
