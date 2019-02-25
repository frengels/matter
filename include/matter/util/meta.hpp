#ifndef MATTER_UTIL_META_HPP
#define MATTER_UTIL_META_HPP

#pragma once

#include <type_traits>

namespace matter
{
namespace detail
{
/// \brief same as enable_if_t but with is_same check, for void_t
template<typename T1, typename T2, typename U = void>
using enable_if_same_t = std::enable_if_t<std::is_same_v<T1, T2>, U>;

/// \brief true if it's a specialization, false otherwise
template<typename, template<typename...> typename>
struct is_specialization_of : std::false_type
{};

template<template<typename...> typename TTemplate, typename... Ts>
struct is_specialization_of<TTemplate<Ts...>, TTemplate> : std::true_type
{};

/// \brief true if we can find `T` in `Ts...`, false otherwise
template<typename T, typename... Ts>
struct type_in_list : std::false_type
{};

template<typename T, typename TT, typename... Ts>
struct type_in_list<T, TT, Ts...> : type_in_list<T, Ts...>
{};

template<typename T, typename... Ts>
struct type_in_list<T, T, Ts...> : std::true_type
{};

template<typename T, typename... Ts>
constexpr bool type_in_list_v = type_in_list<T, Ts...>::value;

/// Check whether all elements of the tuple `TTup` are present in the
/// provides `Ts...`
template<typename TTup, typename... Ts>
struct tuple_in_list : std::false_type
{};

template<typename... T1s, typename... T2s>
struct tuple_in_list<std::tuple<T1s...>, T2s...>
    : std::conjunction<type_in_list<T1s, T2s...>...>
{};

template<typename TTup, typename... Ts>
constexpr bool tuple_in_list_v = tuple_in_list<TTup, Ts...>::value;

namespace impl
{
template<typename Tup, typename... Ts>
struct merge_non_void_impl
{};

template<typename... Filtered>
struct merge_non_void_impl<std::tuple<Filtered...>>
{
    using type = std::tuple<Filtered...>;
};

template<typename... Filtered, typename... Ts>
struct merge_non_void_impl<std::tuple<Filtered...>, void, Ts...>
    : merge_non_void_impl<std::tuple<Filtered...>, Ts...>
{};

template<typename... Filtered, typename T, typename... Ts>
struct merge_non_void_impl<std::tuple<Filtered...>, T, Ts...>
    : merge_non_void_impl<std::tuple<Filtered..., T>, Ts...>
{};
} // namespace impl

/// \brief merge all non `false_type` types into a tuple
template<typename... Ts>
struct merge_non_void : impl::merge_non_void_impl<std::tuple<>, Ts...>
{};

template<typename... Ts>
using merge_non_void_t = typename merge_non_void<Ts...>::type;

} // namespace detail
} // namespace matter

#endif
