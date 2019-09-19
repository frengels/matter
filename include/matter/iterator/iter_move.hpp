#pragma once

#include <utility>

#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace iter_move_impl
{
template<typename I>
constexpr auto
    iter_move(priority_tag<4>,
              I&& it) noexcept(noexcept(iter_move(std::forward<I>(it))))
        -> decltype(iter_move(std::forward<I>(it)))
{
    return iter_move(std::forward<I>(it));
}

template<typename I> // clang-format off
    requires
        requires(I&& it)
        {
            requires std::is_lvalue_reference_v<decltype(*std::forward<I>(it))>;
        } // clang-format on
constexpr auto
    iter_move(priority_tag<3>,
              I&& it) noexcept(noexcept(std::move(*std::forward<I>(it))))
        -> decltype(std::move(*std::forward<I>(it)))
{
    return std::move(*std::forward<I>(it));
}

template<typename I> // clang-format off
    requires
        requires(I&& it)
        {
            requires !std::is_lvalue_reference_v<decltype(*std::forward<I>(it))>;
        }
constexpr auto // clang-format on
    iter_move(priority_tag<3>, I&& it) noexcept(noexcept(*std::forward<I>(it)))
        -> decltype(*std::forward<I>(it))
{
    return *std::forward<I>(it);
}
} // namespace iter_move_impl

inline namespace cpo
{
struct iter_move_fn
{
    template<typename I>
    constexpr auto operator()(I&& it) const noexcept(
        noexcept(::matter::iter_move_impl::iter_move(max_priority_tag,
                                                     std::forward<I>(it))))
        -> decltype(::matter::iter_move_impl::iter_move(max_priority_tag,
                                                        std::forward<I>(it)))
    {
        return ::matter::iter_move_impl::iter_move(max_priority_tag,
                                                   std::forward<I>(it));
    }
};

constexpr auto iter_move = iter_move_fn{};
} // namespace cpo
} // namespace matter
