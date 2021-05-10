#!/usr/bin/env python
import threading
import os
import rospy
import smach
import roslib
from std_srvs.srv import Empty
from std_msgs.msg import String
from agribot_msgs.msg import NavSatFixOrientation
from agribot_msgs.msg import AgribotMoveBaseCommand
from agribot_msgs.srv import ToolManagerControlSource, ToolManagerControlSourceRequest
from agribot_common.agribot_parameters import get_parameter
from agribot_commander.state_machine.state_autonomous.pathpointsstuffer import (PathPointsStuffer)
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

LOG_TAG = "StateMachine"


def wait_for_planners():
    rate = rospy.Rate(10)  # ROS Rate at 10Hz
    waypoint_planner_inited = False
    simple_planner_inited = False
    while not (rospy.has_param('/agribot/waypoint_global_planer/is_inited')
               and rospy.has_param('/agribot/agribot_local_planer/is_inited')):
        try:
            rate.sleep()
        except rospy.ROSInterruptException as exc:
            rospy.logerr("[{}]: {}".format(LOG_TAG, exc))
    while not (waypoint_planner_inited and simple_planner_inited):
        waypoint_planner_inited = \
        rospy.get_param('/agribot/waypoint_global_planer/is_inited')
        simple_planner_inited = \
        rospy.get_param('/agribot/agribot_local_planer/is_inited')
        try:
            rate.sleep()
        except rospy.ROSInterruptException as exc_choose:
            rospy.logerr("[{}]: {}".format(LOG_TAG, exc_choose))


class ChooseTask(smach.State):
    # pylint: disable=too-many-instance-attributes
    """State used to search the tasks database to pick the task for realisation"""
    def __init__(self):
        smach.State.__init__(self,
                             outcomes=['tr_task_is_active', 'tr_task_conf_error'],
                             output_keys=['choose_task_output'])
        self.mutex = threading.Lock()
        self.pub = rospy.Publisher('agribot/waypoints', NavSatFixOrientation, queue_size=1000)
        self.pub_move_base_command = rospy.Publisher('/agribot/navigation_manager/command',
                                                     AgribotMoveBaseCommand,
                                                     queue_size=10)
        self.choosen_path_name = ''
        self.received_new_task = False
        self.new_task_name = ''
        try:
            pth_location_folder = os.path.join(os.environ.get('AGRIBOT_HOME'),
                                               get_parameter('/agribot_map/paths_path'))
        except ValueError as exception_msg:
            rospy.logerr("[{}] {}".format(LOG_TAG, exception_msg))
            return
        self.points_handler = PathPointsStuffer(pth_location_folder)
        self.pub_state = rospy.Publisher('/agribot/state_machine/state', String, queue_size=10)
        self.state_msg = String()

    def clear_previous_path(self):
        navigation_manager_ready = False
        # send the ABORT to clear the current task
        move_base_commands_msg = AgribotMoveBaseCommand()
        move_base_commands_msg.command = AgribotMoveBaseCommand.ABORT
        self.pub_move_base_command.publish(move_base_commands_msg)
        rate = rospy.Rate(10)  # ROS Rate at 10 Hz
        rospy.loginfo("[StateMachine] Waiting for the navigation_manager to clear the previous task")
        while not navigation_manager_ready:
            if rospy.has_param('/agribot/navigation_manager/is_ready'):
                navigation_manager_ready = rospy.get_param('/agribot/navigation_manager/is_ready')
            try:
                rate.sleep()
            except rospy.ROSInterruptException as exc_choose:
                rospy.logerr("[StateMachine]: " + str(exc_choose))
        rospy.loginfo("[StateMachine] Previous path cleared from navigation_manager")

    def load_the_chosen_path(self):
        function_output = ["transition", "user_data"]
        path_loaded = False
        while path_loaded or self.new_task_name != '':
            if self.new_task_name != '':
                if not self.points_handler.set_path_name(str(self.new_task_name)):
                    rospy.logerr("[StateMachine] Path {} does not exists.".format(
                        self.new_task_name))
                    send_last_msg = False
                    self.new_task_name = ''
                    path_loaded = False
                    rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
                    function_output[0] = 'tr_task_conf_error'
                    function_output[1] = 'task_config_error'
                else:
                    self.points_handler.get_path_points_from_yaml()
                    goal_fix = NavSatFixOrientation()
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
                        rospy.loginfo("[StateMachine] Path name: " + \
                                      rospy.get_param('/agribot/chosen_task_name') + \
                                      " loaded succesfully")
                        self.pub.publish(goal_fix)
                        send_last_msg = False
                        path_loaded = False
                        self.points_handler.reset_path_point_stuffer()
                        rospy.set_param('/agribot/chosen_task_name', '')
                        function_output[0] = 'tr_task_is_active'
                        function_output[1] = 'task_is_active'
        return function_output

    def execute(self, ud):  # pylint: disable=invalid-name
        """Pick and return the proper transition. Choose the proper the task name picked
        by the user or if it cant be found return to manual or safe state"""
        output_transition = ''
        rospy.loginfo("[StateMachine] Executing state: ChooseTask INPUT: none")
        self.state_msg = "ChooseTask"
        self.pub_state.publish(self.state_msg)
        self.mutex.acquire()
        if rospy.has_param('/agribot/chosen_task_name'):
            self.new_task_name = rospy.get_param('/agribot/chosen_task_name')
        else:
            self.new_task_name = ''
        self.mutex.release()
        #Check if we received any task name
        if self.new_task_name != '':
            #Check if move_base planner nodes are ready, else wait
            rospy.loginfo("[StateMachine] Waiting for planners ")
            wait_for_planners()
            rospy.loginfo("[StateMachine] Planners are ready, now loading the chosen path ")
            self.clear_previous_path()
            #Check if task name is ok and if the task itself is proper
            function_output = self.load_the_chosen_path()
            ud.choose_task_output = function_output[1]
            output_transition = function_output[0]
        else:
            ud.choose_task_output = 'task_config_error'
            rospy.set_param('/agribot/robot_current_state', 'manual_control_state')
            output_transition = 'tr_task_conf_error'
        rospy.loginfo("[StateMachine] Leaving state: ChooseTask TRANSITION: " +
                      str(output_transition))
        if output_transition == 'tr_task_is_active':
            rospy.wait_for_service('/cmd_vel_arbiter_node/switch_to_navigation')
            switch_to_navigation = rospy.ServiceProxy('/cmd_vel_arbiter_node/switch_to_navigation',
                                                      Empty)
            tool_arbiter = rospy.ServiceProxy('/agribot/tool_management/control_source',
                                              ToolManagerControlSource)
            try:
                switch_to_navigation()
                tool_arbiter(ToolManagerControlSourceRequest.NAVIGATION)
            except rospy.ServiceException as exc:
                rospy.logerr("[{}] Service did not process request: {}".format(LOG_TAG, exc))
        return output_transition
