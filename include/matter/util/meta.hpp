#ifndef MATTER_UTIL_META_HPP
#define MATTER_UTIL_META_HPP

#pragma once

#include <optional>
#include <type_traits>

namespace matter
{
namespace detail
{
namespace impl
{
template<typename T, std::size_t... Is, typename... Args>
constexpr T construct_from_tuple_impl(
    std::index_sequence<Is...>,
    std::in_place_type_t<T>,
    std::tuple<Args...>
        targs) noexcept(std::is_nothrow_constructible_v<T, Args...>)
{
    return T(std::forward<Args>(std::get<Is>(targs))...);
}
} // namespace impl

/// \brief create T from a forward_as_tuple
template<typename T, typename TupArgs>
constexpr T construct_from_tuple(std::in_place_type_t<T>,
                                 TupArgs&& targs) noexcept
{
    return impl::construct_from_tuple_impl(
        std::make_index_sequence<std::tuple_size<TupArgs>::value>{},
        std::in_place_type_t<T>{},
        std::forward<TupArgs>(targs));
}

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

namespace impl
{
template<std::size_t I, typename T, typename U, typename... Ts>
struct search_type_impl : search_type_impl<I + 1, T, Ts...>
{};

template<std::size_t I, typename T, typename... Ts>
struct search_type_impl<I, T, T, Ts...> : std::integral_constant<std::size_t, I>
{};
} // namespace impl

/// \brief return the index of the type or false_type
template<typename T, typename... Ts>
struct search_type : impl::search_type_impl<0, T, Ts...>
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

/// \brief get index of type or nullopt
template<typename T, typename... Ts>
constexpr std::optional<std::size_t> type_index() noexcept
{
    if constexpr (type_in_list_v<T, Ts...>)
    {
        return search_type<T, Ts...>::value;
    }
    else
    {
        return std::nullopt;
    }
}

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

template<std::size_t N, typename... Ts>
struct nth
{};

template<std::size_t N, typename T, typename... Ts>
struct nth<N, T, Ts...> : nth<N - 1, Ts...>
{};

template<typename T, typename... Ts>
struct nth<0, T, Ts...>
{
    using type = T;
};

template<std::size_t N, typename... Ts>
using nth_t = typename nth<N, Ts...>::type;

template<typename T, typename TupArgs>
struct is_constructible_expand_tuple;

template<typename T, typename... Args>
struct is_constructible_expand_tuple<T, std::tuple<Args...>>
    : std::is_constructible<T, Args...>
{};

template<typename T, typename TupArgs>
constexpr auto is_constructible_expand_tuple_v =
    is_constructible_expand_tuple<T, TupArgs>::value;

template<typename T, typename TupArgs>
struct is_nothrow_constructible_expand_tuple;

template<typename T, typename... Args>
struct is_nothrow_constructible_expand_tuple<T, std::tuple<Args...>>
    : std::is_nothrow_constructible<T, Args...>
{};

template<typename T, typename TupArgs>
constexpr auto is_nothrow_constructible_expand_tuple_v =
    is_nothrow_constructible_expand_tuple<T, TupArgs>::value;
} // namespace detail
} // namespace matter

#endif
