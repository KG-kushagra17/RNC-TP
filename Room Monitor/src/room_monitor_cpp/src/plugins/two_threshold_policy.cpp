
#include "room_monitor_cpp/alert_policy.hpp"

namespace room_monitor_cpp
{

class TwoThresholdPolicy : public AlertPolicy
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
        int exceeded = 0;

        if (temperature > temperature_threshold) {
            ++exceeded;
        }

        if (humidity > humidity_threshold) {
            ++exceeded;
        }

        if (smoke > smoke_threshold) {
            ++exceeded;
        }

        return exceeded >= 2;
    }
};

}  // namespace room_monitor_cpp

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(
    room_monitor_cpp::TwoThresholdPolicy,
    room_monitor_cpp::AlertPolicy)
