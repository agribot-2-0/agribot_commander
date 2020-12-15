#!/usr/bin/env python

import os
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore
import rospy

from agribot_task_supervisor import AgribotTaskSupervisor


def if_param_exists(param_name):
    if not rospy.has_param(param_name):
        rospy.logerr(
            "[AgribotTaskSupervisor] '{}' param not set".format(param_name))
        return False
    return True


def node():
    rospy.init_node('task_supervisor')
    if if_param_exists('rsn'):
        rsn = rospy.get_param('rsn')
    else:
        return

    if if_param_exists('rsn'):
        rsn = rospy.get_param('rsn')
    else:
        return

    if if_param_exists('certificate_path'):
        certificate_path = rospy.get_param('certificate_path')
    else:
        return

    if if_param_exists('certificate_file'):
        certificate_file = rospy.get_param('certificate_file')
    else:
        return

    # init task handling object:
    key_path = os.path.join(os.environ.get('AGRIBOT_HOME'), certificate_path)
    if not os.path.exists(key_path):
        rospy.logerr(
            "[AgribotTaskSupervisor] Key folder {} not present".format(
                key_path))
        return

    cert_file = os.path.join(key_path, certificate_file)

    if not firebase_admin._apps:  # pylint: disable=protected-access
        cred = credentials.Certificate(cert_file)
        firebase_admin.initialize_app(cred)
        database = firestore.client()

    agribot_task_supervisor = AgribotTaskSupervisor(rsn, database)
    agribot_task_supervisor.run()


if __name__ == '__main__':
    try:
        node()
    except rospy.ROSInterruptException:
        pass
