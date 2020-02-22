#include "animated_value.hh"
#include "color.hh"
#include "object.hh"
#include "offset.hh"

#include <cairo.h>

#include <cmath>
#include <iostream>
#include <iterator> // std::end
#include <list>

namespace Sian {

namespace {

Offset bottom_right_halfdiagonal(const Object& o)
{
    return Offset(o.object_width() / 2, o.object_height() / 2)
                .rotate(o.rotation);
}

Offset bottom_left_halfdiagonal(const Object& o)
{
    return Offset(-o.object_width() / 2, o.object_height() / 2)
                .rotate(o.rotation);
}

} // namespace Sian::{anonymous}

Object::Object(Offset center, Color color, double completion)
    : completion(completion),
      color(color),
      rotation(0.0),
      scale_x(1.0),
      scale_y(1.0),
      center(center),
      offset(
          this->center,
          [this] (Offset center) {
              return center
                        .minus_x(this->x_dimension() / 2)
                        .minus_y(this->y_dimension() / 2);
          },
          [this] (Offset offset) {
              return offset
                        .plus_x(this->x_dimension() / 2)
                        .plus_y(this->y_dimension() / 2);
          }),
      top_left(
          this->center,
          [this] (Offset center) {
              return center.minus(bottom_right_halfdiagonal(*this));
          },
          [this] (Offset top_left) {
              return top_left.plus(bottom_right_halfdiagonal(*this));
          }),
      top_right(
          this->center,
          [this] (Offset center) {
              return center.minus(bottom_left_halfdiagonal(*this));
          },
          [this] (Offset top_right) {
              return top_right.plus(bottom_left_halfdiagonal(*this));
          }),
      bottom_right(
          this->center,
          [this] (Offset center) {
              return center.plus(bottom_right_halfdiagonal(*this));
          },
          [this] (Offset bottom_right) {
              return bottom_right.minus(bottom_right_halfdiagonal(*this));
          }),
      bottom_left(
          this->center,
          [this] (Offset center) {
              return center.plus(bottom_left_halfdiagonal(*this));
          },
          [this] (Offset bottom_left) {
              return bottom_left.minus(bottom_left_halfdiagonal(*this));
          })
{ }

Object::~Object()
{ }

void Object::push_context(DrawContext cr) const
{
    cairo_save(cr);

    cairo_translate(cr, center.get().x, center.get().y);
    cairo_scale(cr, scale_x, scale_y);
    cairo_rotate(cr, rotation.get());
    cairo_translate(cr, -natural_width() / 2, -natural_height() / 2);

    Color c = color.get();
    cairo_set_source_rgba(
            cr,
            c.red() / 255,
            c.green() / 255,
            c.blue() / 255,
            c.alpha() / 255);
}

void Object::pop_context(DrawContext cr) const
{
    cairo_restore(cr);
}

std::list<UpdatableValue*> Object::animated_values()
{
    UpdatableValue* values[] = {
        &completion, &color, &rotation, &scale_x, &scale_y, &center, &offset,
        &top_left, &top_right, &bottom_right, &bottom_left
    };
    return std::list<UpdatableValue*>(values, std::end(values));
}

void Object::step(double time_delta, StepID step_id)
{
    for (UpdatableValue* animated_value : animated_values())
    {
        animated_value->step(time_delta, step_id);
    }
}

void Object::show_creation(const PaceValue& pace_value)
{
    completion.set(0.0).animate_to(1.0, pace_value);
}

double Object::x_dimension() const
{
    return std::abs(object_height() * std::sin(rotation)) + std::abs(object_width() * std::cos(rotation));
}

double Object::y_dimension() const
{
    return std::abs(object_width() * std::sin(rotation)) + std::abs(object_height() * std::cos(rotation));
}

double Object::object_width() const
{
    return natural_width() * scale_x;
}

double Object::object_height() const
{
    return natural_height() * scale_y;
}

} // namespace Sian
