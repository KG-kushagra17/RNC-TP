
import rclpy
from rclpy.node import Node

from room_monitor_interface.msg import SensorReading, Alert


class RoomDashboard(Node):

    def __init__(self):
        super().__init__('room_dashboard')

        self.temperature = None
        self.humidity = None
        self.smoke = None

        self.temperature_sub = self.create_subscription(
            SensorReading, '/temperature',
            self.temperature_callback, 10)

        self.humidity_sub = self.create_subscription(
            SensorReading, '/humidity',
            self.humidity_callback, 10)

        self.smoke_sub = self.create_subscription(
            SensorReading, '/smoke',
            self.smoke_callback, 10)

        self.alert_sub = self.create_subscription(
            Alert, '/alerts',
            self.alert_callback, 10)

        self.get_logger().info('Room dashboard started.')

    def temperature_callback(self, msg):
        self.temperature = msg.value
        self.display_readings()

    def humidity_callback(self, msg):
        self.humidity = msg.value
        self.display_readings()

    def smoke_callback(self, msg):
        self.smoke = msg.value
        self.display_readings()

    def alert_callback(self, msg):
        self.get_logger().warning(
            f'ALERT [{msg.severity}]: {msg.message}')

    def display_readings(self):
        if (self.temperature is not None and
                self.humidity is not None and
                self.smoke is not None):

            self.get_logger().info(
                f'Temperature: {self.temperature:.1f} C | '
                f'Humidity: {self.humidity:.1f}% | '
                f'Smoke: {self.smoke:.2f}'
            )


def main(args=None):
    rclpy.init(args=args)
    node = RoomDashboard()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
