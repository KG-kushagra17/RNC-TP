
#include "rclcpp/rclcpp.hpp"
#include "room_monitor_interface/msg/alert.hpp"

using room_monitor_interface::msg::Alert;

class RoomMonitorAlarm : public rclcpp::Node
{
public:
    RoomMonitorAlarm() : Node("room_monitor_alarm")
    {
        alert_sub_ = this->create_subscription<Alert>(
            "/alerts",
            10,
            std::bind(
                &RoomMonitorAlarm::alert_callback,
                this,
                std::placeholders::_1
            )
        );

        RCLCPP_INFO(this->get_logger(), "Alarm subscriber started.");
    }

private:
    void alert_callback(const Alert::SharedPtr msg)
    {
        if (msg->severity == "CRITICAL") {
            RCLCPP_ERROR(
                this->get_logger(),
                "CRITICAL ALARM: %s | Temp: %.1f C | Humidity: %.1f%% | Smoke: %.2f",
                msg->message.c_str(),
                msg->temperature,
                msg->humidity,
                msg->smoke
            );
        } else {
            RCLCPP_WARN(
                this->get_logger(),
                "ALERT [%s]: %s",
                msg->severity.c_str(),
                msg->message.c_str()
            );
        }
    }

    rclcpp::Subscription<Alert>::SharedPtr alert_sub_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RoomMonitorAlarm>());
    rclcpp::shutdown();
    return 0;
}
