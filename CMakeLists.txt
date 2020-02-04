cmake_minimum_required(VERSION 2.8.3)
project(agribot_commander)

find_package(catkin REQUIRED COMPONENTS
   rospy
   roscpp
   smach
   smach_ros
   std_msgs
   message_generation
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
  ${catkin_INCLUDE_DIRS}
)
