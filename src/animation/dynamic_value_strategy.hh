#ifndef DYNAMIC_VALUE_STRATEGY_HH
#define DYNAMIC_VALUE_STRATEGY_HH

#include "instruction.hh"

#include <functional>
#include <memory>

namespace Sian {

template<typename T>
class DynamicValueStrategy
{
public:
    virtual ~DynamicValueStrategy()
    { }

    virtual double step(double time_delta) = 0;

    virtual T get() const = 0;

    virtual bool is_finite() const = 0;

    virtual StrategyType type() const = 0;

    virtual void add_action(std::function<void()> action)
    {
        action();
    }
};

} // namespace Sian


#endif
