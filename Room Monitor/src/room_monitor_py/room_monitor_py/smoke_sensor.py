import rclpy
from rclpy.node import Node

from room_monitor_interface.msg import SensorReading
from room_monitor_interface.srv import SetSmoke


class SmokeSensor(Node):

    def __init__(self):
        super().__init__('smoke_sensor')

        self.declare_parameter('initial_value', 0.1)

        # Store the current smoke reading
        self.current_value = float(
            self.get_parameter('initial_value').value
        )

        self.publisher = self.create_publisher(
            SensorReading, '/smoke', 10
        )

        self.timer = self.create_timer(
            1.0, self.publish_reading
        )

        # Service to change the smoke reading
        self.set_smoke_service = self.create_service(
            SetSmoke,
            '/set_smoke',
            self.set_smoke_callback
        )

        self.get_logger().info(
            'Smoke sensor started. SetSmoke service ready.'
        )

    def publish_reading(self):
        msg = SensorReading()

        msg.value = self.current_value
        msg.unit = 'normalized_concentration'
        msg.stamp = self.get_clock().now().to_msg()

        self.publisher.publish(msg)

    def set_smoke_callback(self, request, response):
        if request.new_value < 0.0:
            response.success = False
            response.message = 'Smoke value cannot be negative.'
            return response

        self.current_value = request.new_value

        response.success = True
        response.message = (
            f'Smoke value updated to {self.current_value:.2f}'
        )

        self.get_logger().info(response.message)

        return response


def main(args=None):
    rclpy.init(args=args)
    node = SmokeSensor()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()

