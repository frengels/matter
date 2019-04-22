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
    [[maybe_unused]] std::tuple<Args...>
        targs) noexcept(std::is_nothrow_constructible_v<T, Args...>)
{
    return T(std::forward<Args>(std::get<Is>(targs))...);
}
} // namespace impl

/// \brief create T from a forward_as_tuple
template<typename T, typename TupArgs>
constexpr T construct_from_tuple(TupArgs&& targs) noexcept
{
    return impl::construct_from_tuple_impl<T>(
        std::make_index_sequence<std::tuple_size<TupArgs>::value>{},
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

template<typename... Ts>
struct first;

template<typename T, typename... Ts>
struct first<T, Ts...>
{
    using type = T;
};

template<typename... Ts>
using first_t = typename first<Ts...>::type;

template<typename T, typename TupArgs>
struct is_constructible_expand_tuple : std::false_type
{};

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

template<typename Callable, typename TupArgs>
struct invoke_result_expand_tuple;

template<typename Callable, typename... Args>
struct invoke_result_expand_tuple<Callable, std::tuple<Args...>>
    : std::invoke_result<Callable, Args...>
{};

template<typename Callable, typename TupArgs>
using invoke_result_expand_tuple_t =
    typename invoke_result_expand_tuple<Callable, TupArgs>::type;

namespace impl
{
template<std::size_t Cur, std::size_t End, std::size_t... Is>
struct make_index_range_impl : make_index_range_impl<Cur + 1, End, Is..., Cur>
{};

template<std::size_t End, std::size_t... Is>
struct make_index_range_impl<End, End, Is...>
{
    using type = std::index_sequence<Is...>;
};
} // namespace impl

template<std::size_t Begin, std::size_t End>
using make_index_range = typename impl::make_index_range_impl<Begin, End>::type;
} // namespace detail

namespace meta
{
template<typename T>
struct is_tuple : std::false_type
{};

template<typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type
{};

template<typename T>
constexpr bool is_tuple_v = matter::meta::is_tuple<T>::value;

template<typename Tuple>
struct decay_tuple_types;

template<typename... Ts>
struct decay_tuple_types<std::tuple<Ts...>>
{
    using type = std::tuple<std::decay_t<Ts>...>;
};

template<typename Tuple>
using decay_tuple_types_t = typename decay_tuple_types<Tuple>::type;

template<typename... Tuples>
struct merge_tuple_types;

// case for single tuple
template<typename... Ts>
struct merge_tuple_types<std::tuple<Ts...>>
{
    using type = std::tuple<Ts...>;
};

// case for 2 or more tuples, recurse over all of them
template<typename... Tuples, typename... Ts, typename... Us>
struct merge_tuple_types<std::tuple<Ts...>, std::tuple<Us...>, Tuples...>
    : merge_tuple_types<std::tuple<Ts..., Us...>, Tuples...>
{};

template<typename... Tuples>
using merge_tuple_types_t = typename merge_tuple_types<Tuples...>::type;

namespace detail
{
template<template<typename> typename Predicate, typename Tuple, typename... Ts>
struct filter_tuple_types_impl;

// no more types to filter leftover
template<template<typename> typename Predicate, typename... TupTs>
struct filter_tuple_types_impl<Predicate, std::tuple<TupTs...>>
{
    using type = std::tuple<TupTs...>;
};

// apply our predicate
template<template<typename> typename Predicate,
         typename... TupTs,
         typename T,
         typename... Ts>
struct filter_tuple_types_impl<Predicate, std::tuple<TupTs...>, T, Ts...>
    : std::conditional_t<
          Predicate<T>::value,
          filter_tuple_types_impl<Predicate, std::tuple<TupTs..., T>, Ts...>,
          filter_tuple_types_impl<Predicate, std::tuple<TupTs...>, Ts...>>
{};
} // namespace detail

template<template<typename> typename Predicate, typename Tuple>
struct filter_tuple_types;

template<template<typename> typename Predicate, typename... Ts>
struct filter_tuple_types<Predicate, std::tuple<Ts...>>
    : detail::filter_tuple_types_impl<Predicate, std::tuple<>, Ts...>
{};

template<template<typename> typename Predicate, typename Tuple>
using filter_tuple_types_t =
    typename filter_tuple_types<Predicate, Tuple>::type;

namespace detail
{
template<typename T>
struct is_void : std::is_same<void, T>
{};
} // namespace detail

template<typename Tuple>
struct filter_void_tuple_types : filter_tuple_types<detail::is_void, Tuple>
{};

template<template<typename...> typename TemplateType, typename Tuple>
struct expand_tuple_type;

template<template<typename...> typename TemplateType, typename... Ts>
struct expand_tuple_type<TemplateType, std::tuple<Ts...>>
{
    using type = TemplateType<Ts...>;
};

template<template<typename...> typename TemplateType, typename Tuple>
using expand_tuple_type_t =
    typename expand_tuple_type<TemplateType, Tuple>::type;

template<std::size_t N, typename Tuple>
struct nth_tuple_type;

template<std::size_t N, typename... Ts>
struct nth_tuple_type<N, std::tuple<Ts...>> : matter::detail::nth<N, Ts...>
{};

template<std::size_t N, typename Tuple>
using nth_tuple_type_t = typename nth_tuple_type<N, Tuple>::type;
} // namespace meta
} // namespace matter

#endif
