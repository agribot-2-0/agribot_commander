#!/usr/bin/env python

import threading
import time
import rospy

from std_msgs.msg import String
from industrial_msgs.msg import RobotMode


class AgribotTaskSupervisor(object):
    def __init__(self, rsn, database):
        self.callback_done = threading.Event()
        self.task_publisher = rospy.Publisher('/agribot/state_machine/task', String, queue_size=1)
        self.robot_mode_publisher = rospy.Publisher('/agribot/state_machine/mode',
                                                    RobotMode,
                                                    queue_size=1)
        self.rsn = rsn
        self.state_previous = ""
        self.actual_task_name = ""
        self.database = database

        rospy.init_node('task_supervisor')
        rospy.Subscriber('/agribot/state_machine/state', String, self.callback_state)

        rospy.logwarn("[AgribotTaskSupervisor] Waiting for state machine state")
        rospy.wait_for_message('/agribot/state_machine/state', String)

        rospy.logwarn("[AgribotTaskSupervisor] Waiting for task")

        self.database.collection(u'robots').document(self.rsn).on_snapshot(self.on_callback)

    def callback_state(self, msg):
        # after finishing task, delete actual task from tasks list and set actual_task to
        # empty string in Firestore assume that task is completed when previous state is
        # TaskCompletionRaport from SM and actual state is Standby
        if self.state_previous == 'TaskCompletionRaport' and msg.data == 'Standby':
            if self.actual_task_name:
                # TODO for testing purposes do not delete completed task.
                # self.delete_task(self.actual_task_name)
                self.update_actual_task(u'')
                rospy.logwarn("[AgribotTaskSupervisor] Task '{}' completed".format(
                    self.actual_task_name))

        self.state_previous = msg.data

    def delete_task(self, task_name):
        self.database.collection(u'robots').document(self.rsn).collection(u'tasks').document(
            task_name).delete()

    def update_actual_task(self, task_name):
        self.database.collection(u'robots').document(self.rsn).update({u'actual_task': task_name})

    def on_callback(self, doc_snapshot, changes, read_time):  # pylint: disable=unused-argument
        for change in changes:
            if change.type.name == 'ADDED' or change.type.name == 'MODIFIED':
                if self.actual_task_name != change.document.to_dict()['actual_task']:
                    self.actual_task_name = change.document.to_dict()['actual_task']
                    if self.actual_task_name:
                        rospy.logwarn("[AgribotTaskSupervisor] Actual task '{}'".format(
                            self.actual_task_name))
                        self.publish_task(self.actual_task_name)
                        time.sleep(0.1)
                        self.publish_robot_mode(RobotMode.AUTO)
                    else:
                        self.publish_robot_mode(RobotMode.UNKNOWN)

        self.callback_done.set()

    def publish_robot_mode(self, robot_mode):
        robot_mode_msg = RobotMode()
        robot_mode_msg.val = robot_mode
        self.robot_mode_publisher.publish(robot_mode_msg)

    def publish_task(self, task_name):
        actual_task_msg = String()
        actual_task_msg.data = task_name
        self.task_publisher.publish(actual_task_msg)

    @staticmethod
    def run():
        rospy.spin()
