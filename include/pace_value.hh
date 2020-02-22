#ifndef PACE_VALUE_HH
#define PACE_VALUE_HH

#include <chrono>

namespace Sian {

enum class PaceValueType
{
    DURATION_SPECIFIED,
    SPEED_SPECIFIED
};

class PaceValue
{
public:
    PaceValue(double value);

    virtual PaceValueType type() const = 0;

    const double value;
};

class Duration : public PaceValue
{
public:
    Duration(double duration);

    Duration(std::chrono::steady_clock::duration duration);

    PaceValueType type() const override;
};

class Speed : public PaceValue
{
public:
    Speed(double speed);

    PaceValueType type() const override;
};

} // namespace Sian

#endif
