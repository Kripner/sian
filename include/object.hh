#ifndef OBJECT_HH
#define OBJECT_HH

#include "animated_value.hh"
#include "color.hh"
#include "offset.hh"

#include <cairo.h>

#include <functional> // std::reference_wrapper
#include <list>

namespace Sian {

class Object
{
public:
    explicit Object(
            Offset center,
            Color color = Color::white,
            double completion = 1.0);

    Object(const Object& o) = delete;

    virtual ~Object();

    using DrawContext = cairo_t*;

    virtual void draw(DrawContext cr) = 0;

    virtual std::list<UpdatableValue*> animated_values();

    void step(double time_delta, StepID step_id);

    virtual void show_creation(const PaceValue& pace_value = Duration(1.0));

    virtual double x_dimension() const;

    virtual double y_dimension() const;

    virtual double object_width() const;

    virtual double object_height() const;

    AnimatedValue<double> completion;

    AnimatedValue<Color> color;

    AnimatedValue<double> rotation;

    AnimatedValue<double> scale_x;

    AnimatedValue<double> scale_y;

    AnimatedValue<Offset> center;

    AnimatedValue<Offset> offset;

    AnimatedValue<Offset> top_left;

    AnimatedValue<Offset> top_right;

    AnimatedValue<Offset> bottom_right;

    AnimatedValue<Offset> bottom_left;

protected:
    virtual void push_context(DrawContext cr) const;

    virtual void pop_context(DrawContext cr) const;

    virtual double natural_width() const = 0;

    virtual double natural_height() const = 0;
};

} // namespace Sian

#endif
