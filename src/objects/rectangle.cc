#include "rectangle.hh"
#include "offset.hh"

#include <cairo.h>

#include <cmath> // std::min
#include <list>

namespace Sian {

Rectangle::Rectangle(double width, double height)
    : width(width), height(height)
{ }

Rectangle::~Rectangle()
{ }

double max_line_width(double width, double height)
{
    return std::min(width / 2, height / 2);
}

void Rectangle::draw(DrawContext cr)
{
    push_context(cr);

    const double m = max_line_width(width, height);
    const double lw = std::min(m, line_width.get());
    cairo_rectangle(cr, lw / 2, lw / 2, width - lw, height - lw);
    cairo_stroke(cr);

    pop_context(cr);
}

std::list<UpdatableValue*> Rectangle::animated_values()
{
    auto values = Shape::animated_values();
    values.insert(values.end(), {&width, &height});
    return values;
}

double Rectangle::natural_width() const
{
    return width;
}

double Rectangle::natural_height() const
{
    return height;
}

void Rectangle::push_context(DrawContext cr) const
{
    Shape::push_context(cr);
    // alternatives: CAIRO_LINE_JOIN_ROUND, CAIRO_LINE_JOIN_BEVEL
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    const double m = max_line_width(width, height);
    if (line_width > m)
    {
        // override the settings from Shape
        cairo_set_line_width(cr, m);
    }
}

} // namespace Sian
