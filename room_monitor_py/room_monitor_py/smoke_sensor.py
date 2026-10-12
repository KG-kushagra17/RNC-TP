
import rclpy
from rclpy.node import Node

from room_monitor_interface.msg import SensorReading


class SmokeSensor(Node):

    def __init__(self):
        super().__init__('smoke_sensor')

        self.declare_parameter('initial_value', 0.1)

        self.publisher = self.create_publisher(
            SensorReading,
            '/smoke',
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
        msg.unit = 'normalized_concentration'
        msg.stamp = self.get_clock().now().to_msg()

        self.publisher.publish(msg)

        self.get_logger().info(
            f'Smoke concentration: {msg.value}'
        )


def main(args=None):
    rclpy.init(args=args)
    node = SmokeSensor()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
