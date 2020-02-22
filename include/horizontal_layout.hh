#ifndef HORIZONTAL_LAYOUT_HH
#define HORIZONTAL_LAYOUT_HH

#include "animated_value.hh"
#include "object.hh"
#include "offset.hh"

#include <initializer_list>
#include <memory>
#include <vector>

namespace Sian {

class HorizontalLayout : public Object
{
public:
    HorizontalLayout(
            const Offset& center,
            std::initializer_list<std::shared_ptr<Object>> children);

    void draw(DrawContext cr) override;

    virtual std::list<UpdatableValue*> animated_values() override;

    std::vector<std::shared_ptr<Object>> children;

protected:
    virtual double natural_width() const;

    virtual double natural_height() const;
};

} // namespace Sian

#endif
