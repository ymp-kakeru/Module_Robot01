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

include_directories(include
  ${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}_joy_node src/mod01_joy.cpp)
#add_executable(${PROJECT_NAME}_imu_node src/imu_samplinger.cpp)
add_executable(${PROJECT_NAME}_imu_node src/imu_samplinger_new.cpp)

target_link_libraries(
  ${PROJECT_NAME}_joy_node
  ${catkin_LIBRARIES}
)

target_link_libraries(
  ${PROJECT_NAME}_imu_node
  ${catkin_LIBRARIES}
)
