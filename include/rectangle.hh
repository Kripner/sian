#ifndef RECTANGLE_HH
#define RECTANGLE_HH

#include "animated_value.hh"
#include "object.hh"
#include "offset.hh"
#include "shape.hh"

namespace Sian {

class Rectangle : public Shape
{
public:
    Rectangle(double width, double height);

    virtual ~Rectangle();

    virtual void draw(DrawContext cr) override;

    virtual std::list<UpdatableValue*> animated_values() override;

    AnimatedValue<double> width;

    AnimatedValue<double> height;

protected:
    virtual void push_context(DrawContext cr) const override;

    virtual double natural_width() const override;

    virtual double natural_height() const override;
};

} // namespace Sian

#endif
