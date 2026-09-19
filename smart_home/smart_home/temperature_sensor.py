import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32
import random


class TemperatureSensor(Node):

    def __init__(self):
        super().__init__('temperature_sensor')

        self.publisher = self.create_publisher(
            Float32,
            'temperature',
            10
        )

        self.timer = self.create_timer(
            2.0,
            self.publish_temperature
        )

    def publish_temperature(self):
        temperature = random.uniform(20.0, 35.0)

        msg = Float32()
        msg.data = temperature

        self.publisher.publish(msg)

        self.get_logger().info(
            f'Temperature: {temperature:.2f} °C'
        )


def main(args=None):
    rclpy.init(args=args)

    node = TemperatureSensor()

    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()

