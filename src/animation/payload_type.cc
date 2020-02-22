#include "payload_type.hh"

#include <cmath>

namespace Sian {

template<>
double interpolate<double>(const double& origin, const double& target, double t)
{
    return origin + (target - origin) * t;
}

template<>
double difference<double>(const double& a, const double& b)
{
    return std::abs(a - b);
}

} // namespace Sian
