
import rclpy
from rclpy.node import Node

from room_monitor_interface.msg import SensorReading


class HumiditySensor(Node):

    def __init__(self):
        super().__init__('humidity_sensor')

        self.declare_parameter('initial_value', 55.0)

        self.publisher = self.create_publisher(
            SensorReading,
            '/humidity',
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
        msg.unit = '%'
        msg.stamp = self.get_clock().now().to_msg()

        self.publisher.publish(msg)

        self.get_logger().info(
            f'Humidity: {msg.value}{msg.unit}'
        )


def main(args=None):
    rclpy.init(args=args)
    node = HumiditySensor()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
