#include "horizontal_layout.hh"
#include "offset.hh"

#include <cairo.h>

#include <algorithm> // std::max
#include <initializer_list>
#include <memory>

namespace Sian {

HorizontalLayout::HorizontalLayout(
        const Offset& center,
        std::initializer_list<std::shared_ptr<Object>> children)
    : Object(center),
      children(children.begin(), children.end())
{ }

void HorizontalLayout::draw(DrawContext cr)
{
    double dx = 0;
    for (auto& child : children)
    {
        dx += child->x_dimension() / 2;
        double dy = child->y_dimension() / 2;
        child->center.set(Offset(dx, dy));
        child->draw(cr);
        dx += child->x_dimension() / 2;
    }
}

std::list<UpdatableValue*> HorizontalLayout::animated_values()
{
    auto values = Object::animated_values();
    for (auto& child : children)
    {
        auto child_values = child->animated_values();
        values.insert(values.end(), child_values.begin(), child_values.end());
    }
    return values;
}

double HorizontalLayout::natural_width() const
{
    double width = 0;
    for (const auto& child : children)
    {
        width += child->x_dimension();
    }
    return width;
}

double HorizontalLayout::natural_height() const
{
    double height = 0;
    for (const auto& child : children)
    {
        height = std::max(height, child->y_dimension());
    }
    return height;
}

} // namespace Sian
