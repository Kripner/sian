#ifndef ANIMATED_VALUE_HH
#define ANIMATED_VALUE_HH

#include "pace_value.hh"

#include <cstdint> // std::uint64_t
#include <functional>
#include <memory>
#include <ostream>

namespace Sian {

using StepID = std::uint64_t;

class UpdatableValue
{
public:
    virtual void step(double time_delta, StepID step_id) = 0;
};

template<typename T>
class AnimatedValue : public UpdatableValue
{
private:
    using This = AnimatedValue<T>&;

    using ValueConvertor = std::function<T(T)>;

    using ValueSupplier = std::function<T(double)>;

    using Action = std::function<void()>;

public:
    AnimatedValue(const T& value);

    AnimatedValue(const AnimatedValue<T>& other);

    AnimatedValue(
            const AnimatedValue<T>& other,
            ValueConvertor from_other_data,
            ValueConvertor to_other_data);

    AnimatedValue(AnimatedValue<T>&&);

    AnimatedValue<T>& operator=(AnimatedValue<T>&&);

    This operator=(const T& rhs);

    operator const T() const;

    T get() const;

    This set(const T& new_value);

    This set(const T& new_value, const Duration& duration);

    This then_set(const T& new_value);

    This then_set(const T& new_value, const Duration& duration);

    This animate_to(const T& target, const PaceValue& pace_value);

    This then_animate_to(const T& target, const PaceValue& pace_value);

    This bind(const ValueSupplier& value_supplier);

    This bind(const ValueSupplier& value_supplier, const Duration& duration);

    This then_bind(const ValueSupplier& value_supplier);

    This then_bind(const ValueSupplier& value_supplier, const Duration& duration);

    This then(const Action& action);

    This connect(const AnimatedValue<T>& other);

    This connect(const AnimatedValue<T>& other,
                 ValueConvertor from_other_data,
                 ValueConvertor to_other_data);

    This cancel_planned();

    This freeze();

    void step(double time_delta, StepID step_id) override;

    template<typename U>
    friend std::ostream& operator<<(std::ostream& stream, const AnimatedValue<U>& animated_value);

private:
    struct Data;

    struct DataWrapper;

    std::shared_ptr<DataWrapper> data_wrapper;

    std::shared_ptr<Data>& data();

    const std::shared_ptr<Data>& data() const;
};

} // namespace Sian

#endif
