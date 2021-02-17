## ! DO NOT MANUALLY INVOKE THIS setup.py, USE CATKIN INSTEAD

from distutils.core import setup
from catkin_pkg.python_setup import generate_distutils_setup

# fetch values from package.xml
setup_args = generate_distutils_setup(packages=[
    'agribot_commander', 'agribot_commander.task_supervisor',
    'agribot_commander.state_machine', 'agribot_commander.state_machine.state_safe',
    'agribot_commander.state_machine.state_autonomous', 'agribot_commander.state_machine.state_manual'
],
                                      package_dir={'': 'scripts'})

setup(**setup_args)