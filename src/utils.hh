#ifndef UTILS_HH
#define UTILS_HH

#include <cstdio> // std::snprintf, std::size_t
#include <memory>
#include <string>
#include <tuple>
#include <typeinfo> // std::bad_cast
#include <type_traits>
#include <utility> // std::forward
#include <vector>

namespace Sian {
namespace Utils {

// printf-like formatting, but returns result instead of printing
template<typename ...Args>
std::string str_format(const std::string& s, Args... args)
{
    // when called with 0, snprintf only returns result size without writing to the buffer
    // (add 1 for \0)
    std::size_t result_size = std::snprintf(nullptr, 0, s.c_str(), args...) + 1;
    std::vector<char> buffer;
    buffer.resize(result_size);
    std::snprintf(&buffer[0], result_size, s.c_str(), args...);
    return std::string(&buffer[0], result_size - 1);
}

// version for std::shared_ptr exists in <memory> under the same name
template<typename T, typename U>
std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& r)
{
    if (auto p = dynamic_cast<T*>(r.get()))
    {
        r.release();
        return std::unique_ptr<T>(p);
    }
    else
    {
        throw std::bad_cast();
    }
}

template<typename... Fs>
struct FuncComposition
{
public:
    FuncComposition(Fs&&... functions)
        : functions(std::forward<Fs>(functions)...)
    { }

    template<typename T>
    auto operator()(T&& value) const
    {
        return call(
                std::integral_constant<std::size_t, 0>(),
                std::forward<T>(value));
    }
private:
    const std::tuple<Fs...> functions;
    static constexpr std::size_t N = sizeof...(Fs);

    template<std::size_t I, typename T>
    auto call(std::integral_constant<std::size_t, I> _, T&& value) const
    {
        return call(
                std::integral_constant<std::size_t, I + 1>(),
                std::get<I>(functions)(std::forward<T>(value)));
    }

    template<typename T>
    auto call(std::integral_constant<std::size_t, N - 1> _, T&& value) const
    {
        return std::get<N - 1>(functions)(std::forward<T>(value));
    }
};

// only suitable for functions expecting one argument and returning a value
template<typename... Fs>
auto compose(Fs... functions)
{
    static_assert(sizeof...(Fs) > 0, "at lease one function is composed");
    return FuncComposition<Fs...>(std::forward<Fs>(functions)...);
}

// std::identity exists since C++20
template<typename T>
T identity(T x)
{
    return x;
}

} // namespace Sian::Utils
} // namespace Sian

#endif
