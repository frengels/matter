#pragma once

#include "matter/container/size.hpp"
#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/iterator/traits.hpp"

namespace matter
{
namespace detail
{
template<typename C, typename = void>
struct maybe_const_iterator
{
    using const_iterator = void;
};

template<typename C>
struct maybe_const_iterator<C, std::void_t<matter::iterator_t<const C>>>
{
    using const_iterator = matter::iterator_t<const C>;
};

template<typename R, typename = void>
struct is_sized : std::false_type
{};

template<typename R>
struct is_sized<R, std::void_t<decltype(matter::size(std::declval<R&>()))>>
    : std::true_type
{};

template<typename C, typename = void>
struct has_shrink_to_fit : std::false_type
{};

template<typename C>
struct has_shrink_to_fit<
    C,
    std::void_t<decltype(std::declval<C&>().shrink_to_fit())>> : std::true_type
{};

template<typename C, typename = void>
struct has_reserve : std::false_type
{};

template<typename C>
struct has_reserve<C,
                   std::void_t<decltype(std::declval<C&>().reserve(
                       std::declval<std::size_t>()))>> : std::true_type
{};

template<typename C, typename = void>
struct has_front : std::false_type
{};

template<typename C>
struct has_front<C, std::void_t<decltype(std::declval<C&>().front())>>
    : std::true_type
{};

template<typename C, typename = void>
struct has_back : std::false_type
{};

template<typename C>
struct has_back<C, std::void_t<decltype(std::declval<C&>().back())>>
    : std::true_type
{};
} // namespace detail

template<typename C>
struct container_traits : detail::maybe_const_iterator<C>
{
    using container_type = C;

    using iterator = iterator_t<C>;
    using sentinel = sentinel_t<C>;

    using value_type      = range_value_t<C>;
    using reference       = range_reference_t<C>;
    using const_reference = range_reference_t<const C>;

    static constexpr bool is_sized = detail::is_sized<C>::value;

    static constexpr void reserve(
        [[maybe_unused]] container_type& c,
        [[maybe_unused]] std::size_t
            new_capacity) noexcept(detail::has_reserve<container_type>::value ?
                                       noexcept(c.reserve(new_capacity)) :
                                       true)
    {
        if constexpr (detail::has_reserve<container_type>::value)
        {
            c.reserve(new_capacity);
        }
    }

    static constexpr void
    shrink_to_fit([[maybe_unused]] container_type& c) noexcept(
        detail::has_shrink_to_fit<container_type>::value ?
            noexcept(c.shrink_to_fit()) :
            true)
    {
        if constexpr (detail::has_shrink_to_fit<container_type>::value)
        {
            c.shrink_to_fit();
        }
    }
};
} // namespace matter
