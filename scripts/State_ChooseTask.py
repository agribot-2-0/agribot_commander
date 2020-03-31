#!/usr/bin/env python
import threading
from sensor_msgs.msg import NavSatFix
import rospy
import smach
import roslib
import rospkg
from pathpointsstuffer import PathPointsStuffer
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

def wait_for_planners():
    rate = rospy.Rate(10) # ROS Rate at 10Hz
    waypoint_planner_inited = False
    simple_planner_inited = False
    while not (rospy.has_param('/agribot/waypoint_global_planer/is_inited') and
               rospy.has_param('/agribot/simple_local_planer/is_inited')):
        rate.sleep()
    while not (waypoint_planner_inited and
               simple_planner_inited):
        waypoint_planner_inited = \
        rospy.get_param('/agribot/waypoint_global_planer/is_inited')
        simple_planner_inited = \
        rospy.get_param('/agribot/simple_local_planer/is_inited')
        rate.sleep()

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
        self.output_string = ''

    def load_the_chosen_path(self):
        path_loaded = False
        choose_task_output = ''
        while path_loaded or self.new_task_name != '':
            if self.new_task_name != '':
                if not self.points_handler.set_path_name(str(self.new_task_name)):
                    rospy.logerr("[State Machine] Path " + \
                                 str(self.new_task_name) + \
                                 "does not exists.")
                    send_last_msg = False
                    self.new_task_name = ''
                    path_loaded = False
                    choose_task_output = 'task_config_error'
                    rospy.set_param('/agribot/robot_current_state',
                                    'manual_control_state')
                    self.output_string = 'tr_task_conf_error'
                else:
                    self.points_handler.get_path_points_from_yaml()
                    goal_fix = NavSatFix()
                    goal_fix.header.frame_id = "ll"
                    send_last_msg = True
                    rospy.set_param('/agribot/current_task_name',
                                    self.new_task_name)
                    self.new_task_name = ''
                    path_loaded = True
            if path_loaded:
                if self.points_handler.are_path_points_availible():
                    goal_fix.header.stamp = rospy.get_rostime()
                    goal_fix = self.points_handler.fill_new_msg_fix(goal_fix)
                    self.pub.publish(goal_fix)
                else:
                    if send_last_msg:
                        rospy.loginfo("[State Machine] Path name: " + \
                                      rospy.get_param('/agribot/chosen_task_name') + \
                                      " loaded succesfully")
                        self.pub.publish(goal_fix)
                        send_last_msg = False
                        path_loaded = False
                        rospy.set_param('/agribot/chosen_task_name', '')
                        choose_task_output = 'task_is_active'
                        self.output_string = 'tr_task_is_active'
        return choose_task_output

    def execute(self, ud): # pylint: disable=invalid-name
        """Pick and return the proper transition. Choose the proper the task name picked
        by the user or if it cant be found return to manual or safe state"""
        self.output_string = ''
        string_msg = "[State Machine] Executing state ChooseTask "
        rospy.loginfo(string_msg)
        self.mutex.acquire()
        if rospy.has_param('/agribot/chosen_task_name'):
            self.new_task_name = rospy.get_param('/agribot/chosen_task_name')
        else:
            self.new_task_name = ''
        self.mutex.release()
        #Check if we received any task name
        if self.new_task_name != '':
            #Check if move_base planner nodes are ready, else wait
            string_msg = "[State Machine] Waiting for planners "
            rospy.loginfo(string_msg)
            wait_for_planners()
            string_msg = "[State Machine] Planners are ready, now loading the chosen path path "
            rospy.loginfo(string_msg)
            #Check if task name is ok and if the task itself is proper
            ud.choose_task_output = self.load_the_chosen_path()
        else:
            ud.choose_task_output = 'task_config_error'
            rospy.set_param('/agribot/robot_current_state',
                            'manual_control_state')
            self.output_string = 'tr_task_conf_error'
        return self.output_string
