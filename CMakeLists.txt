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
  message_generation
  agribot_common
  agribot_msgs
  roscpp
  rospy
  tf2
  tf2_ros
  std_msgs
  sensor_msgs
  actionlib_msgs
  move_base_msgs
  visualization_msgs
)

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

add_executable(navsatfix_to_local_node src/navsatfix_to_local_node/navsatfix_to_local_node.cpp src/navsatfix_to_local_node/NavSatFixToLocalNode.cpp)
target_link_libraries(navsatfix_to_local_node ${catkin_LIBRARIES})
add_dependencies(navsatfix_to_local_node ${catkin_EXPORTED_TARGETS})

add_executable(move_base_monitor_node src/move_base_monitor_node/move_base_monitor_node.cpp src/move_base_monitor_node/MoveBaseMonitor.cpp)
target_link_libraries(move_base_monitor_node ${catkin_LIBRARIES})
add_dependencies(move_base_monitor_node ${catkin_EXPORTED_TARGETS})

#############
## Install ##
#############

install(TARGETS
  navsatfix_to_local_node
  move_base_monitor_node
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

catkin_install_python(PROGRAMS
	scripts/state_machine/AgribotStateMachine.py
	scripts/utilities/pathpointsstuffer.py
  scripts/utilities/path_points_stuffer_node.py
  scripts/utilities/config_reader.py
  scripts/state_machine/autonomous_control/State_AutonomousControl_Start.py
	scripts/state_machine/autonomous_control/State_AutonomousTaskMonitor.py
  scripts/state_machine/autonomous_control/State_TaskIsBlocked.py
  scripts/state_machine/autonomous_control/State_IsTaskChoosen.py
  scripts/state_machine/autonomous_control/State_ChooseTask.py
  scripts/state_machine/autonomous_control/State_TaskCompletionRaport.py
  scripts/state_machine/manual_control/State_ManualControl_Start.py
  scripts/state_machine/manual_control/State_AutonomousControlTransition.py
  scripts/state_machine/manual_control/State_MobileAppConnectionRoutine.py
  scripts/state_machine/manual_control/State_MobileAppRoutine_Start.py
  scripts/state_machine/safe_state/State_Standby.py
	scripts/state_machine/safe_state/State_SafeState_Start.py
	scripts/state_machine/safe_state/State_EngineStop.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  PATTERN ".svn" EXCLUDE
)

#############
## Testing ##
#############
