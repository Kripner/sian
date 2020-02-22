#include "object.hh"
#include "rectangle_container.hh"
#include "offset.hh"

#include <cairo.h>

#include <memory>

namespace Sian {

RectangleContainer::RectangleContainer(std::shared_ptr<Object> child, double padding)
    : Rectangle(0, 0),
      padding(padding),
      child(child)
{ }

RectangleContainer::~RectangleContainer()
{ }

void RectangleContainer::draw(DrawContext cr)
{
    // update rectangle's dimensions based on those of child
    width = natural_width();
    height = natural_height();
    Rectangle::draw(cr);

    push_context(cr);
    child->offset = Offset(padding, padding);
    child->draw(cr);
    pop_context(cr);
}

std::list<UpdatableValue*> RectangleContainer::animated_values()
{
    auto values = Rectangle::animated_values();
    auto child_values = child->animated_values();
    values.insert(values.end(), child_values.begin(), child_values.end());
    return values;
}

double RectangleContainer::natural_width() const
{
    return child->object_width() + 2 * padding;
}

double RectangleContainer::natural_height() const
{
    return child->object_height() + 2 * padding;
}

} // namespace Sian
