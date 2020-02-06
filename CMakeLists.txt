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
)

add_message_files(
  FILES
  mobile_app_frame.msg
)

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
  ${catkin_INCLUDE_DIRS}
  ${agribot_common_SOURCE_DIR}/include
)

add_executable(hal_mobile_app_commands src/hal_mobile_app_commands/hal_mobile_app_commands.cpp src/hal_mobile_app_commands/AppFrameInterpreter.cpp)
target_link_libraries(hal_mobile_app_commands  ${catkin_LIBRARIES})
add_dependencies(hal_mobile_app_commands agribot_commander_generate_messages_cpp)


