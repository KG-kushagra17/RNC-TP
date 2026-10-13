
cmake_minimum_required(VERSION 3.8)
project(room_monitor_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pluginlib REQUIRED)
find_package(room_monitor_interface REQUIRED)

# Common interface header for the alert policy plugins
include_directories(include)

# Existing room monitoring controller
add_executable(room_monitor_controller
  src/room_monitor_controller.cpp
)

ament_target_dependencies(room_monitor_controller
  rclcpp
  room_monitor_interface
  pluginlib
)

# Existing C++ alarm subscriber
add_executable(room_monitor_alarm
  src/room_monitor_alarm.cpp
)

ament_target_dependencies(room_monitor_alarm
  rclcpp
  room_monitor_interface
)

# Existing C++ SetSmoke service client
add_executable(smoke_service_client
  src/smoke_service_client.cpp
)

ament_target_dependencies(smoke_service_client
  rclcpp
  room_monitor_interface
)

# Plugin A: alert if any threshold is exceeded
add_library(any_threshold_policy SHARED
  src/plugins/any_threshold_policy.cpp
)

ament_target_dependencies(any_threshold_policy
  pluginlib
)

# Plugin B: alert if at least two thresholds are exceeded
add_library(two_threshold_policy SHARED
  src/plugins/two_threshold_policy.cpp
)

ament_target_dependencies(two_threshold_policy
  pluginlib
)

# Make the common interface header available when building
# and after installing the package
target_include_directories(any_threshold_policy PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(two_threshold_policy PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Install executables
install(TARGETS
  room_monitor_controller
  room_monitor_alarm
  smoke_service_client
  DESTINATION lib/${PROJECT_NAME}
)

# Install shared plugin libraries
install(TARGETS
  any_threshold_policy
  two_threshold_policy
  LIBRARY DESTINATION lib
)

# Install the policy interface header
install(DIRECTORY include/
  DESTINATION include
)

# Install plugin description XML
install(FILES plugins/alert_plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

# Register plugins with pluginlib
pluginlib_export_plugin_description_file(
  room_monitor_cpp
  plugins/alert_plugins.xml
)

ament_export_include_directories(include)
ament_export_dependencies(pluginlib)

ament_package()

