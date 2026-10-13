
#ifndef ROOM_MONITOR_CPP__ALERT_POLICY_HPP_
#define ROOM_MONITOR_CPP__ALERT_POLICY_HPP_

namespace room_monitor_cpp
{

class AlertPolicy
{
public:
    virtual ~AlertPolicy() = default;

    virtual bool should_alert(
        double temperature,
        double humidity,
        double smoke,
        double temperature_threshold,
        double humidity_threshold,
        double smoke_threshold) = 0;
};

}  // namespace room_monitor_cpp

#endif  // ROOM_MONITOR_CPP__ALERT_POLICY_HPP_
