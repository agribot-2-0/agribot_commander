cmake_minimum_required(VERSION 2.8.3)
project(agribot_commander)

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
)

#add_message_files(
#  FILES
#)

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES agribot_msgs
 CATKIN_DEPENDS message_runtime std_msgs
#  DEPENDS system_lib
)

include_directories(
  include/hal_mobile_app_commands
  include
  ${catkin_INCLUDE_DIRS}
  ${agribot_common_SOURCE_DIR}/include
)

add_executable(hal_mobile_app_commands_node src/hal_mobile_app_commands/hal_mobile_app_commands_node.cpp src/hal_mobile_app_commands/AppFrameInterpreter.cpp)
target_link_libraries(hal_mobile_app_commands_node  ${catkin_LIBRARIES})
add_dependencies(hal_mobile_app_commands_node agribot_commander_generate_messages_cpp)


add_executable(navsatfix_to_local_node src/navsatfix_to_local_node/navsatfix_to_local_node.cpp src/navsatfix_to_local_node/NavSatFixToLocalNode.cpp)
target_link_libraries(navsatfix_to_local_node ${catkin_LIBRARIES})
add_dependencies(navsatfix_to_local_node ${catkin_EXPORTED_TARGETS})

add_executable(hal_mobile_app_commands_can_node src/hal_mobile_app_commands/hal_mobile_app_commands_node_can.cpp ${agribot_common_SOURCE_DIR}/src/Can.cpp src/hal_mobile_app_commands/AppFrameInterpreter_can.cpp src/hal_mobile_app_commands/AppFrameInterpreter.cpp)
target_link_libraries(hal_mobile_app_commands_can_node ${catkin_LIBRARIES})
add_dependencies(hal_mobile_app_commands_can_node ${catkin_EXPORTED_TARGETS})