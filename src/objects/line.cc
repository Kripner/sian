#include "line.hh"
#include "offset.hh"

#include <algorithm> // std::min
#include <cmath> // std::abs
#include <list>

namespace Sian {

Line::Line(Offset start, Offset end, double width)
    : Shape((start + end) / 2, width),
      start(start),
      end(end)
{
    // This asymmetry leads to unintuitive behaviour, but will be hard to get rid of.
    center.connect(
        this->end,
        [this] (Offset end) {
            return end + (this->start.get() - end) / 2;
        },
        [this] (Offset center) {
            return center - (this->start.get() - this->end.get()) / 2;
        });
}

Line::~Line()
{ }

void Line::draw(DrawContext cr)
{
    push_context(cr);

    const Offset start_norm =
        Offset(
            start.get().x - std::min(start.get().x, end.get().x),
            start.get().y - std::min(start.get().y, end.get().y));

    cairo_move_to(cr, start_norm.x, start_norm.y);
    double dx = end.get().x - start.get().x;
    double dy = end.get().y - start.get().y;
    cairo_line_to(
        cr,
        start_norm.x + dx * completion.get(),
        start_norm.y + dy * completion.get());
    cairo_stroke(cr);

    pop_context(cr);
}

std::list<UpdatableValue*> Line::animated_values()
{
    auto values = Shape::animated_values();
    values.insert(values.end(), {&start, &end});
    return values;
}

double Line::natural_width() const
{
    return std::abs(start.get().x - end.get().x);
}

double Line::natural_height() const
{
    return std::abs(start.get().y - end.get().y);
}

} // namespace Sian
