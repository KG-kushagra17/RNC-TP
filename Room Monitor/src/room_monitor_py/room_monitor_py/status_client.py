
import rclpy
from rclpy.node import Node

from room_monitor_interface.srv import GetStatus


class StatusClient(Node):

    def __init__(self):
        super().__init__('status_client')

        self.client = self.create_client(
            GetStatus, '/get_status'
        )

        self.get_logger().info('Waiting for GetStatus service...')

        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service unavailable; retrying...')

        self.request_status()

    def request_status(self):
        request = GetStatus.Request()

        self.future = self.client.call_async(request)
        self.future.add_done_callback(self.response_callback)

    def response_callback(self, future):
        try:
            response = future.result()

            self.get_logger().info(
                f'Status: {response.status}\n'
                f'Temperature: {response.temperature:.1f} C\n'
                f'Humidity: {response.humidity:.1f}%\n'
                f'Smoke: {response.smoke:.2f}'
            )

        except Exception as error:
            self.get_logger().error(
                f'Service call failed: {error}'
            )

        finally:
            rclpy.shutdown()


def main(args=None):
    rclpy.init(args=args)
    node = StatusClient()

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
