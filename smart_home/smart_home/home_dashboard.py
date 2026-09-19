import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32
from std_srvs.srv import SetBool


class HomeDashboard(Node):

    def __init__(self):
        super().__init__('home_dashboard')

        self.temperature = None
        self.plug_state = False

        self.subscription = self.create_subscription(
            Float32,
            'temperature',
            self.temperature_callback,
            10
        )

        self.client = self.create_client(
            SetBool,
            'plug_control'
        )

        self.get_logger().info('Home Dashboard started')

    def temperature_callback(self, msg):

        self.temperature = msg.data

        self.get_logger().info(
            f'Received temperature: {self.temperature:.2f} °C'
        )

        if self.temperature > 30.0 and not self.plug_state:
            self.control_plug(True)

        elif self.temperature < 25.0 and self.plug_state:
            self.control_plug(False)

    def control_plug(self, state):

        if not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().warn('Smart Plug service not available')
            return

        request = SetBool.Request()
        request.data = state

        future = self.client.call_async(request)

        future.add_done_callback(
            lambda future: self.service_response(future, state)
        )

    def service_response(self, future, state):

        try:
            response = future.result()

            if response.success:
                self.plug_state = state
                self.get_logger().info(response.message)

        except Exception as e:
            self.get_logger().error(
                f'Service call failed: {e}'
            )


def main(args=None):

    rclpy.init(args=args)

    node = HomeDashboard()

    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
