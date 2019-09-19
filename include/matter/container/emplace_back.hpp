#pragma once

#include <utility>

#include "matter/ranges/traits.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace emplace_back_impl
{
template<typename C, typename... Args>
constexpr auto emplace_back(priority_tag<4>, C& cont, Args&&... args) noexcept(
    noexcept(cont.emplace_back(std::forward<Args>(args)...)))
    -> decltype(cont.emplace_back(std::forward<Args>(args)...))
{
    return cont.emplace_back(std::forward<Args>(args)...);
}

template<typename C, typename... Args>
constexpr auto emplace_back(priority_tag<3>, C& cont, Args&&... args) noexcept(
    noexcept(emplace_back(cont, std::forward<Args>(args)...)))
    -> decltype(emplace_back(cont, std::forward<Args>(args)...))
{
    return emplace_back(cont, std::forward<Args>(args)...);
}

template<typename C, typename U>
constexpr auto emplace_back(priority_tag<2>, C& cont, U&& u) noexcept(
    noexcept(cont.push_back(std::forward<U>(u))))
    -> decltype(cont.push_back(std::forward<U>(u)))
{
    return cont.push_back(std::forward<U>(u));
}

template<typename C, typename... Args>
constexpr auto emplace_back(priority_tag<1>, C& cont, Args&&... args) noexcept(
    noexcept(cont.push_back(range_value_t<C>(std::forward<Args>(args)...))))
    -> decltype(cont.push_back(range_value_t<C>(std::forward<Args>(args)...)))
{
    return cont.push_back(range_value_t<C>(std::forward<Args>(args)...));
}
} // namespace emplace_back_impl

inline namespace cpo
{
struct emplace_back_fn
{
    template<typename C, typename... Args>
    constexpr auto operator()(C& cont, Args&&... args) const noexcept(noexcept(
        ::matter::emplace_back_impl::emplace_back(max_priority_tag,
                                                  cont,
                                                  std::forward<Args>(args)...)))
        -> decltype(::matter::emplace_back_impl::emplace_back(
            max_priority_tag,
            cont,
            std::forward<Args>(args)...))
    {
        return ::matter::emplace_back_impl::emplace_back(
            max_priority_tag, cont, std::forward<Args>(args)...);
    }
};

constexpr auto emplace_back = emplace_back_fn{};
} // namespace cpo
} // namespace matter