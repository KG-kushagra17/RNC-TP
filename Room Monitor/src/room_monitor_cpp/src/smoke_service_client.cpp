
#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "room_monitor_interface/srv/set_smoke.hpp"

using namespace std::chrono_literals;
using SetSmoke = room_monitor_interface::srv::SetSmoke;

class SmokeServiceClient : public rclcpp::Node
{
public:
    SmokeServiceClient()
    : Node("smoke_service_client")
    {
        client_ = this->create_client<SetSmoke>("/set_smoke");

        // Request the value through a configurable parameter
        this->declare_parameter<double>("new_value", 0.8);
    }

    void send_request()
    {
        if (!client_->wait_for_service(5s)) {
            RCLCPP_ERROR(
                this->get_logger(),
                "SetSmoke service is unavailable.");
            return;
        }

        auto request = std::make_shared<SetSmoke::Request>();
        request->new_value =
            this->get_parameter("new_value").as_double();

        auto future = client_->async_send_request(request);

        // Wait for the response
        auto result = rclcpp::spin_until_future_complete(
            this->get_node_base_interface(),
            future);

        if (result == rclcpp::FutureReturnCode::SUCCESS) {
            const auto response = future.get();

            if (response->success) {
                RCLCPP_INFO(
                    this->get_logger(),
                    "%s",
                    response->message.c_str());
            } else {
                RCLCPP_WARN(
                    this->get_logger(),
                    "Request rejected: %s",
                    response->message.c_str());
            }
        } else {
            RCLCPP_ERROR(
                this->get_logger(),
                "Failed to receive SetSmoke response.");
        }
    }

private:
    rclcpp::Client<SetSmoke>::SharedPtr client_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SmokeServiceClient>();
    node->send_request();

    rclcpp::shutdown();
    return 0;
}
