import rclpy
from rclpy.node import Node
from std_srvs.srv import SetBool


class SmartPlugController(Node):

    def __init__(self):
        super().__init__('smart_plug_controller')

        self.plug_on = False

        self.service = self.create_service(
            SetBool,
            'plug_control',
            self.control_plug
        )

        self.get_logger().info('Smart Plug Controller ready')

    def control_plug(self, request, response):

        self.plug_on = request.data

        if self.plug_on:
            response.success = True
            response.message = 'Smart Plug turned ON'
            self.get_logger().info('Smart Plug: ON')
        else:
            response.success = True
            response.message = 'Smart Plug turned OFF'
            self.get_logger().info('Smart Plug: OFF')

        return response


def main(args=None):
    rclpy.init(args=args)

    node = SmartPlugController()

    rclpy.spin(node)

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
