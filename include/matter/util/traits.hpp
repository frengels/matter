#ifndef MATTER_UTIL_TRAITS_HPP
#define MATTER_UTIL_TRAITS_HPP

#pragma once

#include <type_traits>

namespace matter
{
namespace impl
{
template<typename... Ts>
struct smallest_;

template<typename T, typename... Ts>
struct smallest_<T, Ts...>
{
    using type = T;
};

template<typename T, typename U, typename... Ts>
struct smallest_<T, U, Ts...>
{
    using type = std::conditional_t<(sizeof(U) < sizeof(T)), U, T>;
};
} // namespace impl

/// \brief holds the smallest of the passed types in ::type
/// if 2 types are the same size the first is picked by default
template<typename... Ts>
struct smallest : impl::smallest_<Ts...>
{};

template<typename... Ts>
using smallest_t = typename smallest<Ts...>::type;

template<typename T, typename U, typename = void>
struct is_sized : std::false_type
{};

template<typename T, typename U>
struct is_sized<
    T,
    U,
    std::void_t<decltype(std::declval<const T>() - std::declval<const U>())>>
    : std::true_type
{};

template<typename T, typename U>
constexpr auto is_sized_v = is_sized<T, U>::value;
} // namespace matter

#endif
