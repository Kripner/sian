#ifndef RECTANGLE_CONTAINER_HH
#define RECTANGLE_CONTAINER_HH

#include "animated_value.hh"
#include "object.hh"
#include "rectangle.hh"

#include <list>
#include <memory>

namespace Sian {

class RectangleContainer : public Rectangle
{
public:
    RectangleContainer(std::shared_ptr<Object> child, double padding = 20);

    virtual ~RectangleContainer();

    virtual void draw(DrawContext cr) override;

    virtual std::list<UpdatableValue*> animated_values() override;

    AnimatedValue<double> padding;

protected:
    double natural_width() const override;

    double natural_height() const override;

private:
    std::shared_ptr<Object> child;
};

} // namespace Sian

#endif
