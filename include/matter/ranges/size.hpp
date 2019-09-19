#pragma once

#include <type_traits>

#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/utility/decay_copy.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace size_impl
{
template<typename I, typename = void>
struct is_integer_like : std::false_type
{};

// not a complete definition of integer-like, see
// http://eel.is/c++draft/iterator.concept.winc for a correct implementation.
template<typename I>
struct is_integer_like<I, std::enable_if_t<std::is_integral_v<I>>>
    : std::true_type
{};

template<typename T, std::size_t N>
constexpr auto
    size(priority_tag<4>,
         T (&arr)[N]) noexcept(noexcept(decay_copy(std::extent_v<T[N]>)))
        -> decltype(decay_copy(std::extent_v<T[N]>))
{
    return decay_copy(std::extent_v<T[N]>);
}

template<typename R>
constexpr auto size(priority_tag<3>, R&& r) noexcept(
    noexcept(decay_copy(std::forward<R>(r).size())))
    -> std::enable_if_t<
        is_integer_like<decltype(decay_copy(std::forward<R>(r).size()))>::value,
        decltype(decay_copy(std::forward<R>(r).size()))>
{
    return decay_copy(std::forward<R>(r).size());
}

// needs to be in scope for the following
template<typename T>
void size(T&&) = delete;

template<typename R>
constexpr auto size(priority_tag<2>, R&& r) noexcept(
    noexcept(decay_copy(size(std::forward<R>(r)))))
    -> std::enable_if_t<
        is_integer_like<decltype(decay_copy(size(std::forward<R>(r))))>::value,
        decltype(decay_copy(size(std::forward<R>(r))))>
{
    return decay_copy(size(std::forward<R>(r)));
}

template<typename R>
using _size_type =
    decltype(decay_copy(matter::end(std::declval<R>() - std::declval<R>())));

template<typename R>
constexpr auto size(priority_tag<1>, R&& r) noexcept(
    noexcept(std::make_unsigned_t<_size_type<R>>{decay_copy(
        matter::end(std::forward<R>(r)) - matter::begin(std::forward<R>(r)))}))
    -> decltype(std::make_unsigned_t<_size_type<R>>{decay_copy(
        matter::end(std::forward<R>(r)) - matter::begin(std::forward<R>(r)))})
{
    return std::make_unsigned_t<_size_type<R>>{decay_copy(
        matter::end(std::forward<R>(r)) - matter::begin(std::forward<R>(r)))};
}
} // namespace size_impl

inline namespace cpo
{
constexpr auto size = [](auto&& range) noexcept(noexcept(
    ::matter::size_impl::size(max_priority_tag,
                              std::forward<decltype(range)>(range))))
                          -> decltype(::matter::size_impl::size(
                              max_priority_tag,
                              std::forward<decltype(range)>(range)))
{
    return ::matter::size_impl::size(max_priority_tag,
                                     std::forward<decltype(range)>(range));
};
} // namespace cpo
} // namespace matter
