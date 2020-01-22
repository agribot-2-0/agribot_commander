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

catkin_package()

include_directories(
  ${catkin_INCLUDE_DIRS}
)
