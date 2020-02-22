#ifndef LINE_HH
#define LINE_HH

#include "animated_value.hh"
#include "object.hh"
#include "offset.hh"
#include "shape.hh"

#include <list>

namespace Sian {

class Line : public Shape
{
public:
    Line(Offset start, Offset end, double line_width = 2.0);

    virtual ~Line();

    virtual void draw(DrawContext cr) override;

    virtual std::list<UpdatableValue*> animated_values() override;

    AnimatedValue<Offset> start;

    AnimatedValue<Offset> end;

protected:
    virtual double natural_width() const;

    virtual double natural_height() const;
};

} // namespace Sian

#endif
