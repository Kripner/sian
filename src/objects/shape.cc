#include "object.hh"
#include "offset.hh"
#include "shape.hh"

#include <list>

namespace Sian {

Shape::Shape(Offset center, double line_width)
    : Object(center), line_width(line_width)
{ }

Shape::~Shape()
{ }

std::list<UpdatableValue*> Shape::animated_values()
{
    auto values = Object::animated_values();
    values.insert(values.end(), {&line_width});
    return values;
}

void Shape::push_context(DrawContext cr) const
{
    Object::push_context(cr);
    cairo_set_line_width(cr, line_width);
}

} // namespace Sian
