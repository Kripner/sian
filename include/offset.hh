#ifndef POSITION_HH
#define POSITION_HH

#include "payload_type.hh"

#include <ostream>

namespace Sian {

class Offset
{
public:
    static const Offset origin;

    Offset(double x, double y);

    Offset plus_x(double val) const;

    Offset plus_y(double val) const;

    Offset plus(const Offset& val) const;

    Offset minus_x(double val) const;

    Offset minus_y(double val) const;

    Offset minus(const Offset& val) const;

    Offset scale(double scalar) const;

    Offset operator+(const Offset& rhs) const;

    Offset operator-(const Offset& rhs) const;

    Offset operator*(double scalar) const;

    Offset operator/(double scalar) const;

    Offset operator-() const;

    Offset rotate(double angle) const;

    double magnitude() const;

    friend std::ostream& operator<<(std::ostream& stream, const Offset& position);

    const double x;
    const double y;
};

template<>
Offset interpolate<Offset>(const Offset& origin, const Offset& target, double t);

template<>
double difference<Offset>(const Offset& a, const Offset& b);

} // namespace Sian

#endif
