#ifndef BASIC_SHAPES_HH
#define BASIC_SHAPES_HH

#include "animated_value.hh"
#include "object.hh"
#include "offset.hh"
#include "shape.hh"

#include <list>

namespace Sian {

class Circle : public Shape
{
public:
    Circle(Offset center, double radius);

    explicit Circle(double radius);

    virtual ~Circle();

    virtual void draw(DrawContext cr) override;

    virtual std::list<UpdatableValue*> animated_values() override;

    AnimatedValue<double> radius;

protected:
    virtual void push_context(DrawContext cr) const override;

    virtual double natural_width() const;

    virtual double natural_height() const;
};

} // namespace Sian

#endif
