#pragma once

#include <type_traits>
#include <utility>

namespace matter
{
template<typename T>
constexpr std::decay_t<T>
decay_copy(T&& t) noexcept(noexcept(std::forward<T>(t)))
{
    return std::forward<T>(t);
}
} // namespace matter
