#ifndef SHAPE_HH
#define SHAPE_HH

#include "animated_value.hh"
#include "object.hh"
#include "offset.hh"

#include <list>

namespace Sian {

class Shape : public Object
{
public:
    Shape(Offset center = Offset(0.0, 0.0), double line_width = 2.0);

    virtual ~Shape();

    virtual std::list<UpdatableValue*> animated_values() override;

    AnimatedValue<double> line_width;

protected:
    virtual void push_context(DrawContext cr) const override;
};

} // namespace Sian

#endif
