
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "room_monitor_interface/msg/sensor_reading.hpp"
#include "room_monitor_interface/msg/alert.hpp"
#include "room_monitor_interface/srv/get_status.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

using SensorReading = room_monitor_interface::msg::SensorReading;
using Alert = room_monitor_interface::msg::Alert;
using GetStatus = room_monitor_interface::srv::GetStatus;

class RoomMonitorController : public rclcpp::Node
{
public:
    RoomMonitorController()
    : Node("room_monitor_controller")
    {
        // Configurable alert thresholds
        this->declare_parameter<double>("temperature_threshold", 40.0);
        this->declare_parameter<double>("humidity_threshold", 80.0);
        this->declare_parameter<double>("smoke_threshold", 0.6);

        // Subscribe to temperature readings
        temperature_sub_ = this->create_subscription<SensorReading>(
            "/temperature", 10,
            [this](SensorReading::SharedPtr msg) {
                temperature_ = msg->value;
                temperature_received_ = true;
                evaluate_alert();
            });

        // Subscribe to humidity readings
        humidity_sub_ = this->create_subscription<SensorReading>(
            "/humidity", 10,
            [this](SensorReading::SharedPtr msg) {
                humidity_ = msg->value;
                humidity_received_ = true;
                evaluate_alert();
            });

        // Subscribe to smoke readings
        smoke_sub_ = this->create_subscription<SensorReading>(
            "/smoke", 10,
            [this](SensorReading::SharedPtr msg) {
                smoke_ = msg->value;
                smoke_received_ = true;
                evaluate_alert();
            });

        // Publish alerts
        alert_pub_ = this->create_publisher<Alert>("/alerts", 10);

        // Provide the latest readings through a service
        status_service_ = this->create_service<GetStatus>(
            "/get_status",
            std::bind(
                &RoomMonitorController::get_status_callback,
                this, _1, _2));

        RCLCPP_INFO(
            this->get_logger(),
            "Room monitoring controller started.");
    }

private:
    // Latest sensor values
    double temperature_{0.0};
    double humidity_{0.0};
    double smoke_{0.0};

    // Track whether each sensor has published
    bool temperature_received_{false};
    bool humidity_received_{false};
    bool smoke_received_{false};

    // Subscriptions
    rclcpp::Subscription<SensorReading>::SharedPtr temperature_sub_;
    rclcpp::Subscription<SensorReading>::SharedPtr humidity_sub_;
    rclcpp::Subscription<SensorReading>::SharedPtr smoke_sub_;

    // Publisher and service
    rclcpp::Publisher<Alert>::SharedPtr alert_pub_;
    rclcpp::Service<GetStatus>::SharedPtr status_service_;

    // Respond to requests for the latest sensor readings
    void get_status_callback(
        const std::shared_ptr<GetStatus::Request> request,
        std::shared_ptr<GetStatus::Response> response)
    {
        (void)request;

        response->temperature = temperature_;
        response->humidity = humidity_;
        response->smoke = smoke_;

        if (!temperature_received_ ||
            !humidity_received_ ||
            !smoke_received_) {
            response->status = "Waiting for sensor readings";
            return;
        }

        response->status = "Room monitoring active";
    }

    // Check whether any configured threshold has been exceeded
    void evaluate_alert()
    {
        // Wait until every sensor has published at least once
        if (!temperature_received_ ||
            !humidity_received_ ||
            !smoke_received_) {
            return;
        }

        const double temp_th =
            this->get_parameter("temperature_threshold").as_double();

        const double hum_th =
            this->get_parameter("humidity_threshold").as_double();

        const double smoke_th =
            this->get_parameter("smoke_threshold").as_double();

        const bool temp_exceeded = temperature_ > temp_th;
        const bool hum_exceeded = humidity_ > hum_th;
        const bool smoke_exceeded = smoke_ > smoke_th;

        // Current policy: alert if ANY threshold is exceeded
        if (temp_exceeded || hum_exceeded || smoke_exceeded) {
            Alert alert;

            alert.message = "Room monitoring threshold exceeded";
            alert.severity = "CRITICAL";

            alert.temperature = temperature_;
            alert.humidity = humidity_;
            alert.smoke = smoke_;

            alert.temp_th = temp_th;
            alert.hum_th = hum_th;
            alert.smoke_th = smoke_th;

            // Convert ROS clock time to the message timestamp
            const auto now = this->get_clock()->now();
            alert.stamp.sec = static_cast<int32_t>(now.seconds());
            alert.stamp.nanosec = static_cast<uint32_t>(
                (now.seconds() - alert.stamp.sec) * 1e9);

            alert_pub_->publish(alert);

            RCLCPP_WARN(
                this->get_logger(),
                "%s",
                alert.message.c_str());
        }
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(
        std::make_shared<RoomMonitorController>());

    rclcpp::shutdown();
    return 0;
}

