#!/usr/bin/env python

import threading
import rospy

from std_msgs.msg import String


class AgribotTaskSupervisor(object):
    def __init__(self, rsn, database):
        self.callback_done = threading.Event()
        self.task_publisher = rospy.Publisher('/agribot/state_machine/task',
                                              String,
                                              queue_size=1)
        self.rsn = rsn
        self.state_previous = ""
        self.actual_task_name = ""
        self.database = database

        rospy.init_node('task_supervisor')
        rospy.Subscriber('/agribot/state_machine/state', String,
                         self.callback_state)
        rospy.wait_for_message('/agribot/state_machine/state', String)

        rospy.loginfo("[AgribotTaskSupervisor] Waiting for task")

        doc_ref = self.database.collection(u'robots').document(self.rsn)
        doc_ref.on_snapshot(self.on_callback)

    def callback_state(self, msg):
        # after finishing task, delete actual task from tasks list and set actual_task to
        # empty string in Firestore assume that task is completed when previous state is
        # TaskCompletionRaport from SM and actual state is Standby
        if self.state_previous == 'TaskCompletionRaport' and msg.data == 'Standby':
            if self.actual_task_name:
                self.database.collection(u'robots').document(
                    self.rsn).collection(u'tasks').document(
                        self.actual_task_name).delete()
                doc_ref = self.database.collection(u'robots').document(
                    self.rsn)
                doc_ref.update({u'actual_task': u''})
                rospy.loginfo(
                    "[AgribotTaskSupervisor] Task '{}' completed".format(
                        self.actual_task_name))

        self.state_previous = msg.data

    def on_callback(self, doc_snapshot, changes, read_time):  # pylint: disable=unused-argument
        for change in changes:
            if change.type.name == 'ADDED' or change.type.name == 'MODIFIED':
                if self.actual_task_name != change.document.to_dict(
                )['actual_task']:
                    self.actual_task_name = change.document.to_dict(
                    )['actual_task']
                    if self.actual_task_name:
                        rospy.loginfo(
                            "[AgribotTaskSupervisor] Actual task '{}'".format(
                                self.actual_task_name))
                        actual_task_msg = String()
                        actual_task_msg.data = self.actual_task_name
                        self.task_publisher.publish(actual_task_msg)

        self.callback_done.set()

    @staticmethod
    def run():
        rospy.spin()
