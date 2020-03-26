#!/usr/bin/env python
from os import path
import threading
from sensor_msgs.msg import NavSatFix
import rospy
import smach
import roslib
import rospkg
from std_msgs.msg import String
from pathpointsstuffer import PathPointsStuffer
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

class ChooseTask(smach.State):
    """State used to search the tasks database to pick the task for realisation"""
    def __init__(self):
        smach.State.__init__(self, outcomes=['tr_task_is_active', 'tr_task_conf_error'],
                             output_keys=['choose_task_output'])
        self.mutex = threading.Lock()
        self.pub = rospy.Publisher('agribot/waypoints', NavSatFix, queue_size=1000)
        self.choosen_path_name = ''
        self.received_new_task = False
        self.new_task_name = ''
        rospack = rospkg.RosPack()
        pth_location_folder = rospack.get_path('agribot_map') + "/paths/"
        self.points_handler = PathPointsStuffer(pth_location_folder)

    def callback_choosentask(self, data):
        self.mutex.acquire()
        self.new_task_name = str(data.data)
        self.received_new_task = True
        self.mutex.release()

    def execute(self, ud):
        """Pick and return the proper transition. Choose the proper the task name picked
        by the user or if it cant be found return to manual or safe state"""
        output_string = ''
        string_msg = "Executing state ChooseTask "
        rospy.loginfo(string_msg)
        self.mutex.acquire()
        if rospy.has_param('/agribot/chosen_task_name'):
            self.new_task_name = rospy.get_param('/agribot/chosen_task_name')
        else:
            self.new_task_name = ''
        self.mutex.release()
        #Check if we received any task name
        if self.new_task_name != '':
            path_loaded = False
            #Check if move_base planner nodes are ready, else wait
            rate = rospy.Rate(10) # ROS Rate at 10Hz
            string_msg = "Waiting for planners "
            rospy.loginfo(string_msg)
            while(True):
                if rospy.has_param('/agribot/waypoint_global_planer/is_inited'):
                    waypoint_planner_inited = rospy.get_param('/agribot/waypoint_global_planer/is_inited')
                    if waypoint_planner_inited:
                        if rospy.has_param('/agribot/simple_local_planer/is_inited'):
                            simple_planner_inited = rospy.get_param('/agribot/simple_local_planer/is_inited')
                            if simple_planner_inited:
                                break
                rate.sleep()
            string_msg = "Planners are ready, now loading the chosen path path "
            rospy.loginfo(string_msg)
            #Check if task name is ok and if the task itself is proper
            while path_loaded or self.new_task_name != '':
                if self.new_task_name != '':
                    if not self.points_handler.set_path_name(str(self.new_task_name)):
                        rospy.logerr("Path " + str(self.new_task_name)+ "does not exists.")
                        send_last_msg = False
                        self.new_task_name = ''
                        path_loaded = False
                        ud.choose_task_output = 'task_config_error'
                        rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                        output_string = 'tr_task_conf_error'
                    else:
                        self.points_handler.get_path_points_from_yaml()
                        goal_fix = NavSatFix()
                        goal_fix.header.frame_id = "ll"
                        send_last_msg = True
                        rospy.set_param('/agribot/current_task_name', self.new_task_name)
                        self.new_task_name = ''
                        path_loaded = True
                if path_loaded:
                    if self.points_handler.are_path_points_availible():
                        goal_fix.header.stamp = rospy.get_rostime()
                        goal_fix = self.points_handler.fill_new_msg_fix(goal_fix)
                        self.pub.publish(goal_fix)
                    else:
                        if send_last_msg:
                            rospy.loginfo("Loading path " + str(self.new_task_name) + " loaded succesfully")
                            self.pub.publish(goal_fix)
                            send_last_msg = False
                            path_loaded = False
                            rospy.set_param('/agribot/chosen_task_name', '')
                            ud.choose_task_output = 'task_is_active'
                            output_string = 'tr_task_is_active'
        else:
            ud.choose_task_output = 'task_config_error'
            rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
            output_string = 'tr_task_conf_error'
        return output_string
