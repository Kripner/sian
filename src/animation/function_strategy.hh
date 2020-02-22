#ifndef FUNCTION_STRATEGY_HH
#define FUNCTION_STRATEGY_HH

#include "dynamic_value_strategy.hh"
#include "instruction.hh"

#include <cmath>
#include <memory>
#include <utility> // std::move

namespace Sian {

template<typename T>
class FunctionStrategy : public DynamicValueStrategy<T>
{
public:
    FunctionStrategy(std::unique_ptr<FunctionInstruction<T>>&& instr_data)
        : instr_data(std::move(instr_data))
    { }

    double step(double time_delta) override
    {
        if (!is_finite())
        {
            relative_time += time_delta;
            return 0.0;
        }
        double time_used = std::min(instr_data->timeout - relative_time, time_delta);
        relative_time += time_used;
        if (time_used < time_delta)
        {
            instr_data->execute_actions();
        }
        return time_used;
    }

    T get() const override
    {
        return instr_data->value_supplier(relative_time);
    }

    bool is_finite() const override
    {
        return instr_data->timeout_type == TimeoutType::WITH_TIMEOUT;
    }

    StrategyType type() const override
    {
        return StrategyType::FUNCTION;
    }

    void add_action(std::function<void()> action) override
    {
        instr_data->add_action(action);
    }

private:
    double relative_time = 0;
    std::unique_ptr<FunctionInstruction<T>> instr_data;
};

} // namespace Sian


#endif
