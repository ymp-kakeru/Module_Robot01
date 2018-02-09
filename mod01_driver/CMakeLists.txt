cmake_minimum_required(VERSION 2.8.3)
project(mod01_driver)

## Add support for C++11, supported in ROS Kinetic and newer
# add_definitions(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  joy
  geometry_msgs
  message_generation
)



## Generate messages in the 'msg' folder
 add_message_files(
   FILES
   mod_joy.msg
   mod_drive.msg
)


## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
 generate_messages(
   DEPENDENCIES
   std_msgs
 )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################


catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES mod01_driver
  CATKIN_DEPENDS roscpp rospy std_msgs joy message_runtime
#  DEPENDS system_lib
)



## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
#add_executable(mod_joy_cntl src/mod_joy_cntl.cpp)
#target_link_libraries(mod_joy_cntl ${catkin_LIBRARIES} )

#add_executable(wheel_state_pub src/wheel_state_pub.cpp)
#target_link_libraries(wheel_state_pub ${catkin_LIBRARIES} )

add_executable(wheel_drive_beta src/wheel_drive_beta.cpp)
target_link_libraries(wheel_drive_beta ${catkin_LIBRARIES} )

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

