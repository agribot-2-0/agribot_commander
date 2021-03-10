#!/usr/bin/env python

import os
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import rospy

from agribot_commander.task_supervisor.agribot_task_supervisor import AgribotTaskSupervisor
from agribot_common.agribot_parameters import get_parameter


def node():
    rospy.init_node('task_supervisor')

    try:
        robot_serial_number = get_parameter('rsn')
        certificate_file = get_parameter('certificate_file')
        certificate_path = get_parameter('certificate_path')
    except ValueError as exception_msg:
        rospy.logerr("[AgribotTaskSupervisor] {}".format(exception_msg))
        return

    # init task handling object:
    key_path = os.path.join(os.environ.get('AGRIBOT_HOME'), certificate_path)
    if not os.path.exists(key_path):
        rospy.logerr("[AgribotTaskSupervisor] Key folder {} not present".format(key_path))
        return

    cert_file = os.path.join(key_path, certificate_file)

    if not firebase_admin._apps:  # pylint: disable=protected-access
        cred = credentials.Certificate(cert_file)
        firebase_admin.initialize_app(cred)
        database = firestore.client()

    agribot_task_supervisor = AgribotTaskSupervisor(robot_serial_number, database)
    agribot_task_supervisor.run()


if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
