#include "animated_value.hh"
#include "animation_strategy.hh"
#include "color.hh"
#include "function_strategy.hh"
#include "instruction.hh"
#include "logger.hh"
#include "offset.hh"
#include "utils.hh"

#include <cstdint> // std::uint64_t
#include <functional>
#include <memory>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <utility> // std::move

namespace Sian {

template<typename T>
struct AnimatedValue<T>::Data
{
    std::unique_ptr<DynamicValueStrategy<T>> strategy;
    std::queue<std::unique_ptr<Instruction<T>>> instructions_queue;
    std::vector<std::weak_ptr<DataWrapper>> connected_wrappers;
    StepID next_step_id = 0;

    void interpret_instruction(std::unique_ptr<Instruction<T>>&& instr)
    {
        switch (instr->strategy_type())
        {
            case StrategyType::ANIMATION:
                strategy = std::make_unique<AnimationStrategy<T>>(
                        strategy->get(),
                        Utils::dynamic_pointer_cast<AnimationInstruction<T>>(std::move(instr)));
                break;
            case StrategyType::FUNCTION:
            case StrategyType::CONSTANT:
                strategy = std::make_unique<FunctionStrategy<T>>(
                        Utils::dynamic_pointer_cast<FunctionInstruction<T>>(std::move(instr)));
                break;
            default:
                throw std::logic_error("Impossible state");
        }
    }

    void push_instruction(std::unique_ptr<Instruction<T>>&& instr)
    {
        if (strategy->is_finite())
        {
            instructions_queue.push(std::move(instr));
        }
        else
        {
            interpret_instruction(std::move(instr));
        }
    }

    void pop_instruction()
    {
        if (instructions_queue.empty())
        {
            set_const(strategy->get());
        }
        else
        {
            interpret_instruction(std::move(instructions_queue.front()));
            instructions_queue.pop();
        }
    }

    void set_const(const T& value)
    {
        strategy = std::make_unique<FunctionStrategy<T>>(
                std::make_unique<ConstantInstruction<T>>(value));
    }

    void step(double time_delta, StepID step_id)
    {
        if (step_id < next_step_id)
            return;
        next_step_id = step_id + 1;

        while (time_delta > 0)
        {
            double time_used = strategy->step(time_delta);
            if (!strategy->is_finite())
                break;
            if (time_used < time_delta)
            {
                time_delta -= time_used;
                pop_instruction();
            }
            else
            {
                time_delta = 0;
            }
        }
    }
};

template<typename T>
struct AnimatedValue<T>::DataWrapper
{
    DataWrapper(
        const std::shared_ptr<Data>& data,
        const ValueConvertor& from_data,
        const ValueConvertor& to_data)
        : data(data), from_data(from_data), to_data(to_data)
    { }

    std::shared_ptr<Data> data;
    ValueConvertor from_data;
    ValueConvertor to_data;
};


template<typename T>
AnimatedValue<T>::AnimatedValue(const T& value)
    : data_wrapper(
          std::make_shared<DataWrapper>(
              std::make_shared<Data>(),
              Utils::identity<T>,
              Utils::identity<T>))
{
    data()->set_const(value);
    data()->connected_wrappers.push_back(
            std::weak_ptr<DataWrapper>(data_wrapper));
}

template<typename T>
AnimatedValue<T>::AnimatedValue(const AnimatedValue<T>& other)
    : data_wrapper(
          std::make_shared<DataWrapper>(*other.data_wrapper))
{
    data()->connected_wrappers.push_back(
            std::weak_ptr<DataWrapper>(data_wrapper));
}

template<typename T>
AnimatedValue<T>::AnimatedValue(
        const AnimatedValue<T>& other,
        ValueConvertor from_other_data,
        ValueConvertor to_other_data)
    : data_wrapper(
          std::make_shared<DataWrapper>(
              other.data_wrapper->data,
              Utils::compose(other.data_wrapper->from_data, from_other_data),
              Utils::compose(to_other_data, other.data_wrapper->to_data)))
{
    data()->connected_wrappers.push_back(
            std::weak_ptr<DataWrapper>(data_wrapper));
}

template<typename T>
AnimatedValue<T>::AnimatedValue(AnimatedValue<T>&&) = default;

template<typename T>
AnimatedValue<T>& AnimatedValue<T>::operator=(AnimatedValue<T>&&) = default;

template<typename T>
auto AnimatedValue<T>::operator=(const T& rhs) -> This
{
    set(rhs);
    return *this;
}

template<typename T>
AnimatedValue<T>::operator const T() const
{
    return get();
}

template<typename T>
T AnimatedValue<T>::get() const
{
    return data_wrapper->from_data(data()->strategy->get());
}

template<typename T>
auto AnimatedValue<T>::set(const T& new_value) -> This
{
    cancel_planned();
    then_set(new_value);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::set(const T& new_value, const Duration& duration) -> This
{
    cancel_planned();
    then_set(new_value, duration);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then_set(const T& new_value) -> This
{
    data()->push_instruction(std::make_unique<ConstantInstruction<T>>(
                data_wrapper->to_data(new_value)));
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then_set(const T& new_value, const Duration& duration) -> This
{
    data()->push_instruction(std::make_unique<ConstantInstruction<T>>(
                data_wrapper->to_data(new_value),
                duration.value));
    return *this;
}

template<typename T>
auto AnimatedValue<T>::animate_to(const T& target, const PaceValue& pace_value) -> This
{
    cancel_planned();
    then_animate_to(target, pace_value);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then_animate_to(const T& target, const PaceValue& pace_value) -> This
{
    data()->push_instruction(std::make_unique<AnimationInstruction<T>>(
                data_wrapper->to_data(target),
                pace_value.type(),
                pace_value.value));
    return *this;
}

template<typename T>
auto AnimatedValue<T>::bind(const ValueSupplier& value_supplier) -> This
{
    cancel_planned();
    then_bind(value_supplier);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::bind(const ValueSupplier& value_supplier, const Duration& duration) -> This
{
    cancel_planned();
    then_bind(value_supplier, duration);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then_bind(const ValueSupplier& value_supplier) -> This
{
    data()->push_instruction(std::make_unique<FunctionInstruction<T>>(
                Utils::compose(value_supplier, data_wrapper->to_data)));
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then_bind(const ValueSupplier& value_supplier, const Duration& duration) -> This
{
    data()->push_instruction(std::make_unique<FunctionInstruction<T>>(
                Utils::compose(value_supplier, data_wrapper->to_data),
                duration.value));
    return *this;
}

template<typename T>
auto AnimatedValue<T>::then(const Action& action) -> This
{
    // attach action to the currently last instruction
    if (data()->instructions_queue.empty())
    {
        data()->strategy->add_action(action);
    }
    else
    {
        data()->instructions_queue.back()->add_action(action);
    }
    return *this;
}

template<typename T>
auto AnimatedValue<T>::connect(const AnimatedValue<T>& other) -> This
{
    connect(other, Utils::identity<T>, Utils::identity<T>);
    return *this;
}

template<typename T>
auto AnimatedValue<T>::connect(const AnimatedValue<T>& other,
             ValueConvertor from_other_data,
             ValueConvertor to_other_data) -> This
{
    if (data() == other.data())
    {
        throw std::logic_error("Can't connect animated values that have "
                               "already been (even indirectly) connected.");
    }
    for (const auto& sibling_weak_ptr : data_wrapper->data->connected_wrappers)
    {
        const auto sibling_wrapper = sibling_weak_ptr.lock();
        if (!sibling_wrapper)
            continue;

        sibling_wrapper->data = other.data();
        sibling_wrapper->from_data = Utils::compose(
                other.data_wrapper->from_data,
                from_other_data,
                data_wrapper->to_data,
                sibling_wrapper->from_data);
        sibling_wrapper->to_data = Utils::compose(
                sibling_wrapper->to_data,
                data_wrapper->from_data,
                to_other_data,
                other.data_wrapper->to_data);

        sibling_wrapper->data->connected_wrappers.push_back(
                    std::weak_ptr<DataWrapper>(sibling_wrapper));
    }
    return *this;
}

template<typename T>
auto AnimatedValue<T>::cancel_planned() -> This
{
    data()->instructions_queue = { };
    return *this;
}

template<typename T>
auto AnimatedValue<T>::freeze() -> This
{
    cancel_planned();
    data()->set_const(data()->strategy->get());
    return *this;
}

template<typename T>
void AnimatedValue<T>::step(double time_delta, StepID step_id)
{
    data()->step(time_delta, step_id);
}

template<typename T>
auto AnimatedValue<T>::data() -> std::shared_ptr<Data>&
{
    return data_wrapper->data;
}

template<typename T>
auto AnimatedValue<T>::data() const -> const std::shared_ptr<Data>&
{
    return data_wrapper->data;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const AnimatedValue<T>& animated_value)
{
    return stream << animated_value.get();
}

// forward declaration for the only allowed payload types

template std::ostream& operator<<<Offset>(std::ostream&, const AnimatedValue<Offset>&);

template std::ostream& operator<<<Color>(std::ostream&, const AnimatedValue<Color>&);

template std::ostream& operator<<<double>(std::ostream&, const AnimatedValue<double>&);


template class AnimatedValue<double>;

template class AnimatedValue<Offset>;

template class AnimatedValue<Color>;

} // namespace Sian
