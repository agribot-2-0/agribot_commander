cmake_minimum_required(VERSION 2.8.3)
project(agribot_commander)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  tf2
  tf2_ros
  std_msgs
  sensor_msgs
)

catkin_package()

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(navsatfix_to_local_node src/navsatfix_to_local_node.cpp src/NavSatFixToLocalNode.cpp)
target_link_libraries(navsatfix_to_local_node ${catkin_LIBRARIES})
add_dependencies(navsatfix_to_local_node ${catkin_EXPORTED_TARGETS})
