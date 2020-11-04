#!/usr/bin/env python

import os
import sys
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import rospy
import rospkg

from agribot_task_supervisor import AgribotTaskSupervisor


def node():
    rsn = sys.argv[1]

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
        database = firestore.client()

    agribot_task_supervisor = AgribotTaskSupervisor(rsn, database)
    agribot_task_supervisor.run()


if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
