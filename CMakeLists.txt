#############################################################################
## Set minimum required version of cmake, project name and compile options ##
#############################################################################
cmake_minimum_required(VERSION 2.8.3)
project(agribot_commander)

## Compilation options
add_compile_options(-std=c++17 -Wall -Wextra -pedantic -Werror)

#############################################################################
## Find catkin packages and libraries for catkin and system dependencies   ##
#############################################################################
find_package(catkin REQUIRED COMPONENTS
  behaviortree_cpp_v3
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


# -------------------BT------------------------------------------------------------------------------------
list(APPEND agribot_behavior_sources src/action/CheckStartConditions.cpp src/action/SwitchControlArbiters.cpp 
    src/action/Handle.cpp src/action/LightsControl.cpp src/action/SwitchTtcontrolState.cpp)
list(APPEND agribot_behavior_sources src/common/SharedField.cpp src/common/OneTimeLogger.cpp)
list(APPEND agribot_behavior_sources src/path_handle/PathParser.cpp)

add_executable(agribot_behavior_node src/node/agribot_behavior_node.cpp src/node/AgribotBehaviorTreeNode.cpp ${agribot_behavior_sources})
target_link_libraries(agribot_behavior_node ${catkin_LIBRARIES} ${behaviortree_cpp_v3_LIBRARIES} yaml-cpp -lstdc++fs)
add_dependencies(agribot_behavior_node ${catkin_EXPORTED_TARGETS})
# ---------------------------------------------------------------------------------------------------------

#############
## Install ##
#############

# -------------------SMACH----------------------------------------------------------------------------------
install(TARGETS
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

catkin_install_python(PROGRAMS
  scripts/agribot_commander/task_supervisor/agribot_task_supervisor_node.py
  scripts/agribot_commander/state_machine/AgribotStateMachine.py

  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}/scripts
)
# ---------------------------------------------------------------------------------------------------------

# -------------------BT------------------------------------------------------------------------------------
install(TARGETS
  agribot_behavior_node
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)
# ---------------------------------------------------------------------------------------------------------

install(DIRECTORY launch behavior_tree
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  PATTERN ".svn" EXCLUDE
)

#############
## Testing ##
#############

include_directories(
  test/include
)

## Add gtest based cpp test target and link libraries
#catkin_add_gtest(${PROJECT_NAME}-test test/utest.cpp ${agribot_behavior_sources})
#target_link_libraries(${PROJECT_NAME}-test ${agribot_common_LIBRARIES} ${behaviortree_cpp_v3_LIBRARIES} gmock gtest)
