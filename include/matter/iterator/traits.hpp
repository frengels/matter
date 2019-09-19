#pragma once

#include <iterator>
#include <type_traits>

#include "matter/iterator/common.hpp"
#include "matter/iterator/iter_move.hpp"

namespace matter
{
namespace detail
{
template<typename T, typename = void>
struct cond_value_type
{};

template<typename T>
struct cond_value_type<T, std::enable_if_t<std::is_object_v<T>>>
{
    using value_type = std::remove_cv_t<T>;
};

template<typename T, typename = void>
struct has_value_type : std::false_type
{};

template<typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type
{};

template<typename T, typename = void>
struct has_element_type : std::false_type
{};

template<typename T>
struct has_element_type<T, std::void_t<typename T::element_type>>
    : std::true_type
{};
} // namespace detail

template<typename T, typename = void>
struct readable_traits
{};

template<typename T>
struct readable_traits<T*, void> : detail::cond_value_type<T>
{};

template<typename T>
struct readable_traits<T, std::enable_if_t<std::is_array_v<T>>>
{
    using value_type = std::remove_cv_t<std::remove_extent_t<T>>;
};

template<typename T>
struct readable_traits<const T, void> : readable_traits<T, void>
{};

template<typename T>
struct readable_traits<T, std::enable_if_t<detail::has_value_type<T>::value>>
    : detail::cond_value_type<typename T::value_type>
{};

template<typename T>
struct readable_traits<T, std::enable_if_t<detail::has_element_type<T>::value>>
    : detail::cond_value_type<typename T::element_type>
{};

namespace detail
{
template<typename I, typename = void>
struct iter_value
{
    using type = typename std::iterator_traits<I>::value_type;
};

template<typename I>
struct iter_value<
    I,
    std::enable_if_t<detail::has_value_type<matter::readable_traits<I>>::value>>
{
    using type = typename matter::readable_traits<I>::value_type;
};
} // namespace detail

template<typename I>
using iter_value_t = typename detail::iter_value<I>::type;

namespace detail
{
template<typename I, typename = void>
struct iter_reference;

template<typename I>
struct iter_reference<I, std::enable_if_t<matter::dereferenceable<I>>>
{
    using type = decltype(*std::declval<I&>());
};
} // namespace detail

template<typename I>
using iter_reference_t = typename detail::iter_reference<I>::type;

template<typename I>
using iter_difference_t = typename std::iterator_traits<I>::difference_type;

namespace detail
{
template<typename I, typename = void>
struct iter_rvalue_reference;

template<typename I>
struct iter_rvalue_reference<
    I,
    std::enable_if_t<
        matter::dereferenceable<I> &&
        can_reference<decltype(matter::iter_move(std::declval<I&>()))>>>
{
    using type = decltype(matter::iter_move(std::declval<I&>()));
};
} // namespace detail

template<typename I>
using iter_rvalue_reference_t = typename detail::iter_rvalue_reference<I>::type;
} // namespace matter
