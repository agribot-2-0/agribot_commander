#!/usr/bin/env python

import threading
import rospy

import actionlib
import google.api_core.exceptions
from agribot_msgs.msg import PerformAutonomousTaskAction
from agribot_msgs.msg import PerformAutonomousTaskGoal
from agribot_msgs.msg import PerformAutonomousTaskFeedback
from agribot_msgs.msg import AgribotNavigationManagerResult


class AgribotTaskSupervisor(object):
    def __init__(self, rsn, database):
        self.callback_done = threading.Event()

        self.task_action_client = actionlib.SimpleActionClient('/agribot/behavior_tree/task_action',
                                                               PerformAutonomousTaskAction)
        self.task_action_client.wait_for_server()

        self.rsn = rsn
        self.actual_task_name = ""
        self.database = database

        rospy.init_node('task_supervisor')
        self.database.collection(u'robots').document(self.rsn).on_snapshot(self.on_callback)

    def delete_task(self, task_name):
        try:
            self.database.collection(u'robots').document(
                self.rsn).collection(u'tasks').document(task_name).delete()
        except google.api_core.exceptions.GoogleAPIError as my_except:
            rospy.logerr("[AgribotTaskSupervisor] Exception: {}".format(my_except))

    def update_actual_task(self, task_name):
        try:
            self.database.collection(u'robots').document(self.rsn).update(
                {u'actual_task': task_name})
        except google.api_core.exceptions.GoogleAPIError as my_except:
            rospy.logerr("[AgribotTaskSupervisor] Exception: {}".format(my_except))

    def on_callback(self, doc_snapshot, changes, read_time):  # pylint: disable=unused-argument
        for change in changes:
            if change.type.name == 'ADDED' or change.type.name == 'MODIFIED':
                if self.actual_task_name != change.document.to_dict()['actual_task']:
                    self.actual_task_name = change.document.to_dict()['actual_task']
                    rospy.logwarn("[AgribotTaskSupervisor] Actual task '{}'".format(
                        self.actual_task_name))
                    self.set_goal_task(self.actual_task_name)
        self.callback_done.set()

    def goal_task_callback_active(self):  # pylint: disable=no-self-use
        # TODO(lchojnacki): implement sending some reaction to goal task action active
        # (eg. sending active status to firestore)
        return

    def goal_task_callback_feedback(self, feedback):  # pylint: disable=unused-argument,no-self-use
        # TODO(lchojnacki): implement sending some reaction to goal task action feedback
        # (eg. sending feedback to firestore)
        feedback_dict = {
            0: 'NOACTION',
            1: 'ACTIVE',
            2: 'PREEMPTED',
            3: 'SUCCES',
            4: 'ABORTED',
            5: 'FAILED',
            7: 'RECALLED',
            10: 'PAUSED',
            100: 'COMPLETED'
        }
        try:
            self.database.collection(u'robots').document(
                self.rsn).collection(u'state').document(u'task').update(
                    {u'feedback': feedback_dict[feedback.feedback.result]})
        except google.api_core.exceptions.GoogleAPIError as my_except:
            rospy.logerr("[AgribotTaskSupervisor] Exception: {}".format(my_except))

        return

    def goal_task_callback_done(self, state, result):  # pylint: disable=unused-argument,no-self-use
        # after finishing task, delete actual task from tasks list and set actual_task to
        # empty string in Firestore assume that task is completed when previous state is
        # TaskCompletionRaport from SM and actual state is Standby
        if self.actual_task_name:
            # TODO for testing purposes do not delete completed task.
            # self.delete_task(self.actual_task_name)
            self.update_actual_task(u'')
            rospy.logwarn("[AgribotTaskSupervisor] Task '{}' completed".format(
                self.actual_task_name))

    def set_goal_task(self, task):
        try:
            goal = PerformAutonomousTaskGoal(task_name=task)
            self.task_action_client.send_goal(goal,
                                              active_cb=self.goal_task_callback_active,
                                              feedback_cb=self.goal_task_callback_feedback,
                                              done_cb=self.goal_task_callback_done)
            rospy.logerr("[AgribotTaskSupervisor] Sent task {}".format(task))
        except Exception as my_except:  # pylint: disable=broad-except
            rospy.logerr("[AgribotTaskSupervisor] Failed to send task: {}".format(my_except))

    @staticmethod
    def run():
        rospy.spin()
