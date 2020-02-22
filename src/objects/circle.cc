#include "circle.hh"

#include <cairo.h>

#include <cmath>
#include <list>

namespace Sian {

Circle::Circle(Offset center, double radius)
    : Shape(center),
      radius(radius)
{ }

Circle::Circle(double radius)
    : Circle(Offset(radius, radius), radius)
{ }

Circle::~Circle()
{ }

void Circle::draw(DrawContext cr)
{
    push_context(cr);

    cairo_translate(cr, radius, radius);
    cairo_new_sub_path(cr);
    const double lw = std::min(radius, line_width);
    cairo_arc(cr, 0.0, 0.0, radius - lw / 2, 0.0, 2 * M_PI * completion);
    cairo_stroke(cr);

    pop_context(cr);
}

double Circle::natural_width() const
{
    return radius * 2;
}

double Circle::natural_height() const
{
    return radius * 2;
}

std::list<UpdatableValue*> Circle::animated_values()
{
    auto values = Shape::animated_values();
    values.insert(values.end(), {&radius, &center});
    return values;
}

void Circle::push_context(DrawContext cr) const
{
    Shape::push_context(cr);
    if (line_width > radius)
    {
        // override the settings from Shape
        cairo_set_line_width(cr, radius);
    }
}

} // namespace Sian
