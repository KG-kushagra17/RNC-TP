
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_loader.hpp"

#include "room_monitor_cpp/alert_policy.hpp"
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

        // Select an alert policy at startup
        this->declare_parameter<std::string>(
            "alert_policy",
            "room_monitor_cpp/AnyThresholdPolicy");

        // Load the plugin interface and selected implementation
        plugin_loader_ =
            std::make_unique<
                pluginlib::ClassLoader<room_monitor_cpp::AlertPolicy>>(
                    "room_monitor_cpp",
                    "room_monitor_cpp::AlertPolicy");

        const auto policy_name =
            this->get_parameter("alert_policy").as_string();

        try {
            alert_policy_ =
                plugin_loader_->createSharedInstance(policy_name);

            RCLCPP_INFO(
                this->get_logger(),
                "Loaded alert policy: %s",
                policy_name.c_str());
        } catch (const pluginlib::PluginlibException & ex) {
            RCLCPP_FATAL(
                this->get_logger(),
                "Failed to load alert policy '%s': %s",
                policy_name.c_str(),
                ex.what());
            throw;
        }

        // Subscribe to temperature
        temperature_sub_ = this->create_subscription<SensorReading>(
            "/temperature", 10,
            [this](SensorReading::SharedPtr msg) {
                temperature_ = msg->value;
                temperature_received_ = true;
                evaluate_alert();
            });

        // Subscribe to humidity
        humidity_sub_ = this->create_subscription<SensorReading>(
            "/humidity", 10,
            [this](SensorReading::SharedPtr msg) {
                humidity_ = msg->value;
                humidity_received_ = true;
                evaluate_alert();
            });

        // Subscribe to smoke
        smoke_sub_ = this->create_subscription<SensorReading>(
            "/smoke", 10,
            [this](SensorReading::SharedPtr msg) {
                smoke_ = msg->value;
                smoke_received_ = true;
                evaluate_alert();
            });

        // Publish alerts
        alert_pub_ = this->create_publisher<Alert>("/alerts", 10);

        // Return the latest sensor readings on request
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
    double temperature_{0.0};
    double humidity_{0.0};
    double smoke_{0.0};

    bool temperature_received_{false};
    bool humidity_received_{false};
    bool smoke_received_{false};

    // Plugin loader must outlive the loaded plugin instance
    std::unique_ptr<
        pluginlib::ClassLoader<room_monitor_cpp::AlertPolicy>>
        plugin_loader_;

    std::shared_ptr<room_monitor_cpp::AlertPolicy> alert_policy_;

    rclcpp::Subscription<SensorReading>::SharedPtr temperature_sub_;
    rclcpp::Subscription<SensorReading>::SharedPtr humidity_sub_;
    rclcpp::Subscription<SensorReading>::SharedPtr smoke_sub_;

    rclcpp::Publisher<Alert>::SharedPtr alert_pub_;
    rclcpp::Service<GetStatus>::SharedPtr status_service_;

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

    void evaluate_alert()
    {
        // Do not evaluate until all sensors have published
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

        // Delegate the decision to the loaded plugin
        const bool should_alert = alert_policy_->should_alert(
            temperature_,
            humidity_,
            smoke_,
            temp_th,
            hum_th,
            smoke_th);

        if (should_alert) {
            Alert alert;

            alert.message = "Room monitoring threshold exceeded";
            alert.severity = "CRITICAL";

            alert.temperature = temperature_;
            alert.humidity = humidity_;
            alert.smoke = smoke_;

            alert.temp_th = temp_th;
            alert.hum_th = hum_th;
            alert.smoke_th = smoke_th;

            const auto now = this->get_clock()->now();
            const double seconds = now.seconds();

            alert.stamp.sec = static_cast<int32_t>(seconds);
            alert.stamp.nanosec = static_cast<uint32_t>(
                (seconds - alert.stamp.sec) * 1e9);

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

