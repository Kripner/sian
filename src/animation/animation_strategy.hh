#ifndef ANIMATION_STRATEGY_HH
#define ANIMATION_STRATEGY_HH

#include "dynamic_value_strategy.hh"
#include "instruction.hh"
#include "pace_value.hh"
#include "payload_type.hh"

#include <memory>
#include <queue>
#include <stdexcept> // std::logic_error
#include <utility> // std::move

namespace Sian {

template<typename T>
class AnimationStrategy : public DynamicValueStrategy<T>
{
public:
    AnimationStrategy(const T& origin, std::unique_ptr<AnimationInstruction<T>>&& instr_data)
        : origin(origin),
          duration(instr_data->value_type == PaceValueType::DURATION_SPECIFIED ?
                       instr_data->value :
                   instr_data->value_type == PaceValueType::SPEED_SPECIFIED ?
                       difference(origin, instr_data->target) / instr_data->value :
                   throw std::logic_error("Impossible state")),
          instr_data(std::move(instr_data))
    { }

    bool is_finite() const override
    {
        return true;
    }

    double step(double time_delta) override
    {
        double time_used;
        if (elapsed + time_delta >= duration)
        {
            time_used = duration - elapsed;
            elapsed = duration;
            instr_data->execute_actions();
        }
        else
        {
            elapsed += time_delta;
            time_used = time_delta;
        }

        return time_used;
    }

    T get() const override
    {
        double t = elapsed / duration;
        return interpolate(origin, instr_data->target, t);
    }

    StrategyType type() const override
    {
        return StrategyType::ANIMATION;
    }

    void add_action(std::function<void()> action) override
    {
        instr_data->add_action(action);
    }

private:
    double elapsed = 0;
    const T origin;
    const double duration;
    std::unique_ptr<AnimationInstruction<T>> instr_data;
};

} // namespace Sian


#endif
