#############################################################################
## Set minimum required version of cmake, project name and compile options ##
#############################################################################
cmake_minimum_required(VERSION 2.8.3)
project(agribot_commander)

#############################################################################
## Find catkin packages and libraries for catkin and system dependencies   ##
#############################################################################
find_package(catkin REQUIRED COMPONENTS
  rospy
  roscpp
  smach
  smach_ros
  std_msgs
  agribot_common
  agribot_msgs
)

catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################
#add_message_files(
#  FILES
#)

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs
#   actionlib_msgs
# )

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES agribot_msgs
# CATKIN_DEPENDS message_runtime std_msgs
#  DEPENDS system_lib
)

#############################################################################
## Build                                                                   ##
#############################################################################

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${agribot_common_SOURCE_DIR}/include
)

link_directories(
 ${PROJECT_SOURCE_DIR}
)

#############
## Install ##
#############

install(TARGETS
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

catkin_install_python(PROGRAMS
  scripts/agribot_commander/task_supervisor/agribot_task_supervisor_node.py
  scripts/agribot_commander/state_machine/AgribotStateMachine.py

  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}/scripts
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  PATTERN ".svn" EXCLUDE
)

#############
## Testing ##
#############
