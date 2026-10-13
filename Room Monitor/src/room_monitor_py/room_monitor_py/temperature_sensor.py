
import rclpy
from rclpy.node import Node

from room_monitor_interface.msg import SensorReading


class TemperatureSensor(Node):

    def __init__(self):
        super().__init__('temperature_sensor')

        self.declare_parameter('initial_value', 28.0)

        self.publisher = self.create_publisher(
            SensorReading,
            '/temperature',
            10
        )

        self.timer = self.create_timer(
            1.0,
            self.publish_reading
        )

    def publish_reading(self):
        msg = SensorReading()

        msg.value = float(
            self.get_parameter('initial_value').value
        )
        msg.unit = 'C'
        msg.stamp = self.get_clock().now().to_msg()

        self.publisher.publish(msg)

        self.get_logger().info(
            f'Temperature: {msg.value} {msg.unit}'
        )


def main(args=None):
    rclpy.init(args=args)
    node = TemperatureSensor()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
