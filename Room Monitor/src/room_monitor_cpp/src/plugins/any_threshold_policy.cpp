
#include "room_monitor_cpp/alert_policy.hpp"

namespace room_monitor_cpp
{

class AnyThresholdPolicy : public AlertPolicy
{
public:
    bool should_alert(
        double temperature,
        double humidity,
        double smoke,
        double temperature_threshold,
        double humidity_threshold,
        double smoke_threshold) override
    {
        return temperature > temperature_threshold ||
               humidity > humidity_threshold ||
               smoke > smoke_threshold;
    }
};

}  // namespace room_monitor_cpp

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(
    room_monitor_cpp::AnyThresholdPolicy,
    room_monitor_cpp::AlertPolicy)
