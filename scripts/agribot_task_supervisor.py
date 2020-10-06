#!/usr/bin/env python

import os
import sys
import threading
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import rospy
import rospkg

from std_msgs.msg import String

CALLBACK_DONE = threading.Event()
TASK_PUBLISHER = rospy.Publisher('/agribot/state_machine/task',
                                 String,
                                 queue_size=1)
RSN = ""
STATE_PREVIOUS = ""
ACTUAL_TASK_NAME = ""
DATABASE = None


def callback_state(msg):
    global STATE_PREVIOUS, ACTUAL_TASK_NAME
    # after finishing task, delete actual task from tasks list and set actual_task to
    # empty string in Firestore assume that task is completed when previous state is
    # TaskCompletionRaport from SM and actual state is Standby
    if STATE_PREVIOUS == 'TaskCompletionRaport' and msg.data == 'Standby':
        if ACTUAL_TASK_NAME:
            DATABASE.collection(u'robots').document(RSN).collection(
                u'tasks').document(ACTUAL_TASK_NAME).delete()
            doc_ref = DATABASE.collection(u'robots').document(RSN)
            doc_ref.update({u'actual_task': u''})

    STATE_PREVIOUS = msg.data


def on_callback(doc_snapshot, changes, read_time):  # pylint: disable=unused-argument
    global ACTUAL_TASK_NAME
    for change in changes:
        if change.type.name == 'ADDED' or change.type.name == 'MODIFIED':
            if ACTUAL_TASK_NAME != change.document.to_dict()['actual_task']:
                ACTUAL_TASK_NAME = change.document.to_dict()['actual_task']
                if ACTUAL_TASK_NAME:
                    rospy.loginfo(
                        "[AgribotTaskSupervisor] Actual task '{}'".format(
                            ACTUAL_TASK_NAME))
                    actual_task_msg = String()
                    actual_task_msg.data = ACTUAL_TASK_NAME
                    TASK_PUBLISHER.publish(actual_task_msg)

    CALLBACK_DONE.set()


def node():
    global DATABASE, RSN
    rospy.init_node('task_supervisor')
    rospy.Subscriber('/agribot/state_machine/state', String, callback_state)
    rospy.wait_for_message('/agribot/state_machine/state', String)

    rospy.loginfo("[AgribotTaskSupervisor] Waiting for task")

    RSN = sys.argv[1]

    # init task handling object:
    rospack = rospkg.RosPack()
    key_path = os.path.join(rospack.get_path('agribot_commander'), "key")
    if not os.path.exists(key_path):
        rospy.logerr("[AgribotTaskSupervisor] Key folder not present.")
        return

    cert_file = os.path.join(
        key_path, "agribot-cd437-firebase-adminsdk-ypr8o-5e1109055d.json")

    if not firebase_admin._apps:  # pylint: disable=protected-access
        cred = credentials.Certificate(cert_file)
        firebase_admin.initialize_app(cred, {
            'projectID': 'agribot-cd437',
        })
        DATABASE = firestore.client()

        doc_ref = DATABASE.collection(u'robots').document(RSN)
        doc_ref.on_snapshot(on_callback)

    rospy.spin()


if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
