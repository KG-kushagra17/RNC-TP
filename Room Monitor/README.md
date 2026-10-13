# ROS 2 Room Monitoring System

A modular room monitoring system built using **ROS 2 Jazzy**, Python, and C++. It monitors temperature, humidity, and smoke readings, evaluates configurable thresholds, publishes alerts, and exposes services for querying room status and changing the smoke sensor value.

## Features

- Three independent Python sensor nodes.
- Custom ROS 2 messages and services.
- C++ monitoring controller with configurable thresholds.
- Python dashboard for live readings and alerts.
- C++ alarm subscriber for critical alerts.
- `GetStatus` service for querying the latest readings.
- `SetSmoke` service for changing the smoke reading.
- C++ `pluginlib` alert policies:
  - `AnyThresholdPolicy`: alert when any threshold is exceeded.
  - `TwoThresholdPolicy`: alert when at least two thresholds are exceeded.

## Architecture

```text
Temperature Sensor ──┐
Humidity Sensor ─────┼──> C++ Controller ──> /alerts
Smoke Sensor ────────┘         │                 │
                               │                 ├──> Python Dashboard
                               │                 └──> C++ Alarm
                               │
                               └──> /get_status

C++ Smoke Client ──> /set_smoke ──> Python Smoke Sensor
```

## Packages

| Package | Purpose |
|---|---|
| `room_monitor_interface` | Custom messages and services |
| `room_monitor_py` | Python sensor nodes, dashboard, and service servers/clients |
| `room_monitor_cpp` | C++ controller, alarm subscriber, service client, and alert plugins |
| `room_monitor_bringup` | Bringup package for system launch configuration |

## Requirements

- Ubuntu 24.04
- ROS 2 Jazzy
- Python 3
- C++ compiler and CMake
- `pluginlib`

## Build

Source ROS 2 and build the workspace:

```bash
source /opt/ros/jazzy/setup.bash

cd ~/rosProjects/Room_monitoring_system

colcon build
source install/setup.bash
```

After building, source the workspace in every new terminal:

```bash
source /opt/ros/jazzy/setup.bash
source ~/rosProjects/Room_monitoring_system/install/setup.bash
```

## Running the system

Start each node in a separate terminal.

### 1. Sensor nodes

```bash
ros2 run room_monitor_py temperature_sensor
```

```bash
ros2 run room_monitor_py humidity_sensor
```

```bash
ros2 run room_monitor_py smoke_sensor
```

### 2. C++ controller

Default policy:

```bash
ros2 run room_monitor_cpp room_monitor_controller
```

Select the policy that requires at least two exceeded thresholds:

```bash
ros2 run room_monitor_cpp room_monitor_controller \
  --ros-args -p alert_policy:=room_monitor_cpp/TwoThresholdPolicy
```

Only one controller instance should run at a time.

### 3. Dashboard

```bash
ros2 run room_monitor_py room_dashboard
```

### 4. C++ alarm subscriber

```bash
ros2 run room_monitor_cpp room_monitor_alarm
```

## Topics

| Topic | Type | Purpose |
|---|---|---|
| `/temperature` | `room_monitor_interface/msg/SensorReading` | Temperature readings |
| `/humidity` | `room_monitor_interface/msg/SensorReading` | Humidity readings |
| `/smoke` | `room_monitor_interface/msg/SensorReading` | Smoke readings |
| `/alerts` | `room_monitor_interface/msg/Alert` | Controller-generated alerts |

## Services

### Get room status

```bash
ros2 service call /get_status \
  room_monitor_interface/srv/GetStatus "{}"
```

Returns the current status, temperature, humidity, and smoke readings.

### Change the smoke value

```bash
ros2 service call /set_smoke \
  room_monitor_interface/srv/SetSmoke "{new_value: 0.8}"
```

The smoke sensor publishes its updated value on `/smoke`.

Run the C++ client with:

```bash
ros2 run room_monitor_cpp smoke_service_client
```

## Parameters

### Sensor parameters

The sensors expose an `initial_value` parameter. For example:

```bash
ros2 param set /temperature_sensor initial_value 35.0
```

### Controller thresholds

```bash
ros2 param set /room_monitor_controller temperature_threshold 40.0
ros2 param set /room_monitor_controller humidity_threshold 80.0
ros2 param set /room_monitor_controller smoke_threshold 0.6
```

The controller compares the latest sensor readings against these thresholds.

## Alert policies

**AnyThresholdPolicy**

Publishes an alert when temperature, humidity, or smoke exceeds its configured threshold.

**TwoThresholdPolicy**

Publishes an alert only when at least two of the three thresholds are exceeded.

The selected policy is loaded at controller startup using `pluginlib`.

## Verification

List running nodes:

```bash
ros2 node list
```

Inspect topics:

```bash
ros2 topic list
```

Inspect the smoke readings:

```bash
ros2 topic echo /smoke
```

List available services:

```bash
ros2 service list
```

Inspect the controller's parameters:

```bash
ros2 param list /room_monitor_controller
```

## Project status

The system includes custom interfaces, Python sensor nodes, a C++ monitoring controller, a dashboard, an alarm subscriber, services, and dynamically loaded alert policies. Build and integration tests should be completed before considering the project release-ready.

## Future improvements

- Launch the full system through a single launch file.
- Add integration tests and automated test coverage.
- Improve alert handling to avoid repeatedly publishing the same active alert.
- Expand generated API documentation and usage examples.
