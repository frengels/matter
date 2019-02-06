#ifndef MATTER_STORAGE_TRAITS_HPP
#define MATTER_STORAGE_TRAITS_HPP

#pragma once

#include <type_traits>

#include "matter/util/meta.hpp"

namespace matter
{
namespace detail
{
template<typename Storage>
using is_storage_sfinae =
    std::void_t<typename Storage::value_type, typename Storage::id_type>;

template<typename Storage>
using is_iterable_sfinae = std::void_t<
    typename Storage::iterator,
    matter::detail::enable_if_same_t<typename Storage::iterator,
                                     decltype(
                                         std::declval<Storage&>().begin())>,
    matter::detail::eanble_if_same_t<typename Storage::iterator,
                                     decltype(std::declval<Storage&>().end())>>;

template<typename Storage>
using is_const_iterable_sfinae =
    std::void_t<typename Storage::const_iterator,
                matter::detail::enable_if_same_t<
                    typename Storage::const_iterator,
                    decltype(std::declval<const Storage&>().begin())>,
                matter::detail::enable_if_same_t<
                    typename Storage::const_iterator,
                    decltype(std::declval<const Sotrage&>().end())>>;

template<typename Storage>
using is_reverse_iterable_sfinae =
    std::void_t<typename Storage::reverse_iterator,
                matter::detail::enable_if_same_t<
                    typename Storage::reverse_iterator,
                    decltype(std::declval<Storage&>().rbegin())>,
                matter::detail::eanble_if_same_t<
                    typename Storage::reverse_iterator,
                    decltype(std::declval<Storage&>().rend())>>;

template<typename Storage>
using is_const_reverse_iterable_sfinae =
    std::void_t<typename Storage::const_reverse_iterator,
                matter::detail::enable_if_same_t<
                    typename Storage::const_reverse_iterator,
                    decltype(std::declval<const Storage&>().rbegin())>,
                matter::detail::enable_if_same_t<
                    typename Storage::const_reverse_iterator,
                    decltype(std::declval<const Sotrage&>().rend())>>;

template<typename Storage, typename Size>
using is_sized_sfinae = std::void_t<
    matter::detail::enable_if_same_t<Size, typename Storage::size_type>,
    matter::detail::enable_if_same_t<
        typename Storage::size_type,
        decltype(std::declval<const Storage&>().size())>>;

template<typename Storage, typename Component, typename... Args>
using is_component_constructible = std::void_t<
    typename Storage::id_type,
    matter::detail::enable_if_same_t<Component, typename Storage::value_type>,
    matter::detail::enable_if_same_t<
        void,
        decltype(std::declval<Storage&>().construct(
            std::declval<typename Storage::id_type>(),
            std::declval<Args&&>()...))>,
    matter::detail::enable_if_same_t<
        void,
        decltype(std::declval<Storage&>().destroy(
            std::declval<typename Storage::id_type>()))>>;

template<typename Storage>
using has_contains_sfinae =
    std::void_t<typename Storage::id_type,
                matter::detail::enable_if_same_t<
                    bool,
                    decltype(std::declval<const Storage&>().contains(
                        std::declval<typename Storage::id_type>()))>>;

} // namespace detail

template<typename Storage, typename = void>
struct is_storage : std::false_type
{};

template<typename Storage>
struct is_storage<Storage, detail::is_storage_sfinae<Storage>> : std::true_type
{};

template<typename Storage>
constexpr bool is_storage_v = is_storage_v<Storage>::value;

template<typename Storage, typename = void>
struct is_storage_iterable : std::false_type
{};

template<typename Storage>
struct is_storage_iterable<Storage, detail::is_iterable_sfinae<Storage>>
    : is_storage<Storage>
{};

template<typename Storage>
constexpr bool is_storage_iterable_v = is_storage_iterable<Storage>::value;

template<typename Storage, typename = void>
struct is_storage_const_iterable : std::false_type
{};

template<typename Storage>
struct is_storage_const_iterable<Storage,
                                 detail::is_const_iterable_sfinae<Storage>>
    : is_storage<Storage>
{};

template<typename Storage>
constexpr bool is_storage_const_iterable_v =
    is_storage_const_iterable<Storage>::value;

template<typename Storage, typename = void>
struct is_storage_reverse_iterable : std::false_type
{};

template<typename Storage>
struct is_storage_reverse_iterable<Storage,
                                   detail::is_reverse_iterable_sfinae<Storage>>
    : is_storage<Storage>
{};

template<typename Storage>
constexpr bool is_storage_reverse_iterable_v =
    is_storage_reverse_iterable<Storage>::value;

template<typename Storage, typename = void>
struct is_storage_const_reverse_iterable : std::false_type
{};

template<typename Storage>
struct is_storage_const_reverse_iterable<
    Storage,
    detail::is_const_reverse_iterable_sfinae<Storage>> : is_storage<Storage>
{};

template<typename Storage>
constexpr bool is_storage_const_reverse_iterable_v =
    is_storage_const_reverse_iterable<Storage>::value;

template<typename Storage, typename = void>
struct is_storage_sized : std::false_type
{};

template<typename Storage>
struct is_storage_sized<Storage, detail::is_sized_sfinae<Storage>>
    : is_storage<Storage>
{};

template<typename Storage>
constexpr bool is_storage_sized_v = is_storage_sized<Storage>::value;

namespace detail
{
template<typename Storage,
         typename Component,
         typename ArgsTuple,
         typename = void>
struct is_storage_component_constructible_impl : std::false_type
{};

template<typename Storage, typename Component, typename... Args>
struct is_storage_component_constructible_impl<
    Storage,
    Component,
    std::tuple<Args...>,
    matter::detail::
        is_storage_component_constructible_sfinae<Storage, Component, Args...>>
    : is_storage<Storage>
{};
} // namespace detail

template<typename Storage, typename Component, typename... Args>
struct is_storage_component_constructible
    : detail::is_component_constructible_impl<Storage,
                                              Component,
                                              std::tuple<Args...>>
{};

template<typename Storage, typename Component, typename... Args>
constexpr bool is_storage_component_constructible_v =
    is_storage_component_constructible<Storage, Component, Args...>::value;
} // namespace matter

#endif
