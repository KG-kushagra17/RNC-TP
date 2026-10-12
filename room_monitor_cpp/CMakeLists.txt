
cmake_minimum_required(VERSION 3.8)
project(room_monitor_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(room_monitor_interface REQUIRED)

add_executable(room_monitor_controller
  src/room_monitor_controller.cpp
)

ament_target_dependencies(room_monitor_controller
  rclcpp
  room_monitor_interface
)

install(TARGETS
  room_monitor_controller
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(room_monitor_alarm
  src/room_monitor_alarm.cpp
)

ament_target_dependencies(room_monitor_alarm
  rclcpp
  room_monitor_interface
)

install(TARGETS
  room_monitor_controller
  room_monitor_alarm
  DESTINATION lib/${PROJECT_NAME}
)
ament_package()

