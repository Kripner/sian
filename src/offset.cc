#include "animation/animation_strategy.hh"
#include "config.hh"
#include "offset.hh"

#include <cmath>

namespace Sian {

const Offset Offset::origin = Offset(0.0, 0.0);

Offset::Offset(double x, double y)
    : x(x), y(y)
{  }

Offset Offset::plus_x(double val) const
{
    return {x + val, y};
}

Offset Offset::plus_y(double val) const
{
    return {x, y + val};
}

Offset Offset::plus(const Offset& val) const
{
    return {x + val.x, y + val.y};
}

Offset Offset::minus_x(double val) const
{
    return plus_x(-val);
}

Offset Offset::minus_y(double val) const
{
    return plus_y(-val);
}

Offset Offset::minus(const Offset& val) const
{
    return plus(-val);
}

Offset Offset::scale(double scalar) const
{
    return {x * scalar, y * scalar};
}

Offset Offset::operator+(const Offset& rhs) const
{
    return plus(rhs);
}

Offset Offset::operator-(const Offset& rhs) const
{
    return minus(rhs);
}

Offset Offset::operator*(double scalar) const
{
    return scale(scalar);
}

Offset Offset::operator/(double scalar) const
{
    return scale(1 / scalar);
}

Offset Offset::operator-() const
{
    return scale(-1);
}

Offset Offset::rotate(double angle) const
{
    using namespace std;
    return {
        x * cos(angle) - y * sin(angle),
        x * sin(angle) + y * cos(angle)
    };
}

double Offset::magnitude() const
{
    return std::sqrt(x * x + y * y);
}

std::ostream& operator<<(std::ostream& stream, const Offset& position)
{
    return stream << "{" << position.x << ", " << position.y << "}";
}

template<>
Offset interpolate<Offset>(const Offset& origin, const Offset& target, double t)
{
    return Offset(
            interpolate(origin.x, target.x, t),
            interpolate(origin.y, target.y, t));
}

template<>
double difference<Offset>(const Offset& a, const Offset& b)
{
    return (a - b).magnitude();
}

} // namespace Sian
