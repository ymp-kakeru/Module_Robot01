cmake_minimum_required(VERSION 2.8.3)
project(mod01_cntl)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
)

catkin_package(

)

add_executable(
  ${PROJECT_NAME}_node src/mod01_joy.cpp
)

target_link_libraries(
  ${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
)
