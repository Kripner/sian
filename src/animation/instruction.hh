#ifndef INSTRUCTION_HH
#define INSTRUCTION_HH

#include "pace_value.hh"

#include <functional>
#include <vector>

namespace Sian {

enum class StrategyType
{
    ANIMATION,
    FUNCTION,
    CONSTANT
};

enum class TimeoutType
{
    WITH_TIMEOUT,
    WITHOUT_TIMEOUT
};

template<typename T>
class Instruction
{
public:
    virtual StrategyType strategy_type() const = 0;

    template<typename F>
    void add_action(const F& action)
    {
        actions.push_back(action);
    }

    void execute_actions()
    {
        for (auto action : actions)
        {
            action();
        }
        actions.clear();
    }

private:
    // actions to be performed after the strategy finishes
    std::vector<std::function<void()>> actions;
};

template<typename T>
class AnimationInstruction : public Instruction<T>
{
public:
    AnimationInstruction(T target, PaceValueType type, double value)
        : target(target), value_type(type), value(value)
    { }

    StrategyType strategy_type() const override
    {
        return StrategyType::ANIMATION;
    }

    const T target;
    const PaceValueType value_type;
    const double value;
};

template<typename T>
using ValueSupplier = std::function<T(double)>;

template<typename T>
class FunctionInstruction : public Instruction<T>
{
public:
    FunctionInstruction(const ValueSupplier<T>& value_supplier)
        : value_supplier(value_supplier),
          timeout_type(TimeoutType::WITHOUT_TIMEOUT),
          timeout(-1)
    { }

    FunctionInstruction(const ValueSupplier<T>& value_supplier, double timeout)
        : value_supplier(value_supplier),
          timeout_type(TimeoutType::WITH_TIMEOUT),
          timeout(timeout)
    { }

    virtual StrategyType strategy_type() const override
    {
        return StrategyType::FUNCTION;
    }

    const ValueSupplier<T> value_supplier;
    const TimeoutType timeout_type;
    const double timeout;
};

template<typename T>
class ConstantInstruction : public FunctionInstruction<T>
{
public:
    ConstantInstruction(const T& value)
        : FunctionInstruction<T>([value] (double _) { return value; })
    { }

    ConstantInstruction(const T& value, double timeout)
        : FunctionInstruction<T>([value] (double _) { return value; }, timeout)
    { }

    StrategyType strategy_type() const override
    {
        return StrategyType::CONSTANT;
    }
};

} // namespace Sian

#endif
