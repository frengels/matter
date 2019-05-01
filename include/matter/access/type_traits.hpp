#ifndef MATTER_ACCESS_TYPE_TRAITS_HPP
#define MATTER_ACCESS_TYPE_TRAITS_HPP

#pragma once

#include <type_traits>

#include "matter/access/entity_handle.hpp"
#include "matter/access/storage_handle.hpp"
#include "matter/component/group_vector.hpp"
#include "matter/util/concepts.hpp"

namespace matter
{
template<typename T, typename = void>
struct processes_group_vector : std::false_type
{};

template<typename T>
struct processes_group_vector<
    T,
    std::void_t<decltype(std::declval<T>().process_group_vector(
        std::declval<matter::group_vector<typename T::id_type>&>()))>>
    : std::true_type
{};

template<typename T>
constexpr bool processes_group_vector_v = processes_group_vector<T>::value;

// gives the result type of invoke process_group_vector on T
template<typename T>
struct process_group_vector_result
{
    using type = decltype(std::declval<T>().process_group_vector(
        std::declval<matter::group_vector<typename T::id_type>&>()));
};

template<typename T>
using process_group_vector_result_t =
    typename process_group_vector_result<T>::type;

template<typename T, typename = void>
struct processes_group : std::false_type
{};

// this is the version where process_group_vector returns void/bool
template<typename T>
struct processes_group<
    T,
    std::enable_if_t<
        std::is_same_v<void, matter::process_group_vector_result_t<T>> ||
            std::is_same_v<bool, matter::process_group_vector_result_t<T>>,
        std::void_t<decltype(std::declval<T>().process_group(
            std::declval<matter::any_group<typename T::id_type>>()))>>>
    : std::true_type
{};

// if process_group_vector returns non void then we dereference its result and
// pass it to process_group
template<typename T>
struct processes_group<
    T,
    std::enable_if_t<
        !std::is_same_v<void, matter::process_group_vector_result_t<T>>,
        std::void_t<decltype(std::declval<T>().process_group(
            std::declval<matter::any_group<typename T::id_type>>(),
            *std::declval<matter::process_group_vector_result_t<T>>()))>>>
    : std::true_type
{};

template<typename T>
constexpr bool processes_group_v = processes_group<T>::value;

template<typename T, typename = void>
struct process_group_result;

// in case the process_group_vector result type is void then we don't pass this
// to process_group
template<typename T>
struct process_group_result<
    T,
    std::enable_if_t<
        std::is_same_v<void, matter::process_group_vector_result_t<T>>>>
{
    using type = decltype(std::declval<T>().process_group(
        std::declval<matter::any_group<typename T::id_type>>()));
};

// in case the process_group_vector result type is not void then we dereference
// it and pass the result to process_group.
template<typename T>
struct process_group_result<
    T,
    std::enable_if_t<
        !std::is_same_v<void, matter::process_group_vector_result_t<T>>>>
{
    using type = decltype(std::declval<T>().process_group(
        std::declval<matter::any_group<typename T::id_type>>(),
        *std::declval<matter::process_group_vector_result_t<T>>()));
};

template<typename T>
using process_group_result_t = typename process_group_result<T>::type;

template<typename T, typename = void>
struct required_types;

namespace detail
{
template<typename MetaAccess, typename TupComponents, typename = void>
struct makes_access_impl : std::false_type
{};

// case where process_group result is void so doesn't get passed
template<typename MetaAccess, typename... Components>
struct makes_access_impl<
    MetaAccess,
    std::tuple<Components...>,
    std::enable_if_t<
        std::is_same_v<void, matter::process_group_result_t<MetaAccess>> ||
            std::is_same_v<bool, matter::process_group_result_t<MetaAccess>>,
        std::void_t<decltype(std::declval<MetaAccess>().make_access(
            std::declval<matter::entity_handle<typename MetaAccess::id_type>>(),
            std::declval<matter::storage_handle<typename MetaAccess::id_type,
                                                Components>>()...))>>>
    : std::true_type
{};

// case where group_vector return isn't void and gets dereferenced when calling
// make_access.
template<typename MetaAccess, typename... Components>
struct makes_access_impl<
    MetaAccess,
    std::tuple<Components...>,
    std::enable_if_t<
        !std::is_same_v<void, matter::process_group_result_t<MetaAccess>> &&
            !std::is_same_v<bool, matter::process_group_result_t<MetaAccess>>,
        std::void_t<decltype(std::declval<MetaAccess>().make_access(
            std::declval<matter::entity_handle<typename MetaAccess::id_type>>(),
            *std::declval<matter::process_group_result_t<MetaAccess>>(),
            std::declval<matter::storage_handle<typename MetaAccess::id_type,
                                                Components>>()...))>>>
    : std::true_type
{};
} // namespace detail

template<typename MetaAccess>
struct makes_access
    : detail::makes_access_impl<MetaAccess,
                                typename required_types<MetaAccess>::type>
{};

template<typename MetaAccess>
constexpr bool makes_access_v = makes_access<MetaAccess>::value;

namespace detail
{
// base template
template<typename MetaAccess, typename TupComponents, typename = void>
struct make_access_result_impl;

// for void/bool return type from process_group
template<typename MetaAccess, typename... Components>
struct make_access_result_impl<
    MetaAccess,
    std::tuple<Components...>,
    std::enable_if_t<
        std::is_same_v<void, matter::process_group_result_t<MetaAccess>> ||
        std::is_same_v<bool, matter::process_group_result_t<MetaAccess>>>>
{
    using type = decltype(std::declval<MetaAccess>().make_access(
        std::declval<matter::entity_handle<typename MetaAccess::id_type>>(),
        std::declval<matter::storage_handle<typename MetaAccess::id_type,
                                            Components>>()...));
};

// case where return from process_group is optional type
template<typename MetaAccess, typename... Components>
struct make_access_result_impl<MetaAccess,
                               std::tuple<Components...>,
                               std::enable_if_t<matter::is_optional_v<
                                   matter::process_group_result_t<MetaAccess>>>>
{
    using type = decltype(std::declval<MetaAccess>().make_access(
        std::declval<matter::entity_handle<typename MetaAccess::id_type>>(),
        *std::declval<matter::process_group_result_t<MetaAccess>>(),
        std::declval<matter::storage_handle<typename MetaAccess::id_type,
                                            Components>>()...));
};

} // namespace detail

template<typename MetaAccess>
struct make_access_result
    : detail::make_access_result_impl<
          MetaAccess,
          typename matter::required_types<MetaAccess>::type>
{};

template<typename MetaAccess>
using make_access_result_t = typename make_access_result<MetaAccess>::type;

template<typename T, typename = void>
struct is_meta_access : std::false_type
{};

template<typename T>
struct is_meta_access<
    T,
    std::enable_if_t<std::is_constructible_v<T, typename T::registry_type&> &&
                         matter::processes_group_v<T> &&
                         matter::makes_access_v<T> &&
                         std::is_same_v<typename T::id_type,
                                        typename T::registry_type::id_type>,
                     std::void_t<typename T::required_types,
                                 typename T::id_type,
                                 typename T::registry_type>>> : std::true_type
{};

template<typename T>
constexpr bool is_meta_access_v = is_meta_access<T>::value;

template<typename T, typename Registry, typename = void>
struct is_access : std::false_type
{};

// access type must indicate a meta type which takes the registry as a
// template parameter. This Registry is passed to extract information like
// id_type mainly.
template<typename T, typename Registry>
struct is_access<
    T,
    Registry,
    std::enable_if_t<
        matter::is_meta_access_v<typename T::template meta_type<Registry>>,
        std::void_t<typename T::template meta_type<Registry>>>> : std::true_type
{};

template<typename T, typename Registry>
constexpr bool is_access_v = is_access<T, Registry>::value;

// wraps required_types in a handy tuple for further processing
template<typename MetaAccess>
struct required_types<MetaAccess>
{
private:
    using _req_types = typename MetaAccess::required_types;

public:
    using type =
        std::conditional_t<matter::meta::is_tuple_v<_req_types>,
                           _req_types,
                           std::conditional_t<std::is_same_v<void, _req_types>,
                                              std::tuple<>,
                                              std::tuple<_req_types>>>;
};

template<typename MetaAccess>
using required_types_t = typename required_types<MetaAccess>::type;

template<typename Access, typename Registry>
using meta_type_t = typename Access::template meta_type<Registry>;
} // namespace matter

#endif
