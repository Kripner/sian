#ifndef PAYLOAD_TYPE_HH
#define PAYLOAD_TYPE_HH

namespace Sian {

template<typename T>
T interpolate(const T& origin, const T& target, double t);

template<typename T>
double difference(const T& a, const T& b);

} // namespace Sian

#endif
