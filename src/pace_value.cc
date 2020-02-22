#include "pace_value.hh"

#include <chrono>

namespace Sian {

PaceValue::PaceValue(double value)
    : value(value)
{ }

Duration::Duration(double duration)
    : PaceValue(duration)
{ }

Duration::Duration(std::chrono::steady_clock::duration duration)
    : PaceValue(
            std::chrono::duration_cast<std::chrono::seconds>(duration).count())
{ }

PaceValueType Duration::type() const
{
    return PaceValueType::DURATION_SPECIFIED;
}

Speed::Speed(double speed)
    : PaceValue(speed)
{ }

PaceValueType Speed::type() const
{
    return PaceValueType::SPEED_SPECIFIED;
}

} // namespace Sian
