#ifndef MATTER_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_TRAITS_HPP

#pragma once

#include <tuple>
#include <type_traits>

#include "matter/util/meta.hpp"

namespace matter
{
namespace detail
{
template<typename Component, typename Id>
using is_storage_defined_sfinae =
    std::void_t<typename Component::template storage_type<Id>>;

template<typename Component>
using is_dependent_sfinae = std::void_t<typename Component::depends_on>;

template<typename Component>
using is_component_sfinae = std::void_t<std::enable_if_t<
    !detail::is_specialization_of<Component, std::tuple>::value>>;

template<typename Component>
using is_variant_sfinae = std::void_t<typename Component::variant_of>;
} // namespace detail

template<typename Component>
struct is_component_empty
    : std::integral_constant<bool, (sizeof(Component) <= 1)>
{};

template<typename Component>
constexpr bool is_component_empty_v = is_component_empty<Component>::value;

template<typename Component, typename Id, typename = void>
struct is_component_storage_defined : std::false_type
{};

template<typename Component, typename Id>
struct is_component_storage_defined<
    Component,
    Id,
    detail::is_storage_defined_sfinae<Component, Id>> : std::true_type
{};

template<typename Component, typename Id>
constexpr bool is_component_storage_defined_v =
    is_component_storage_defined<Component, Id>::value;

template<typename Component, typename = void>
struct is_component_dependent : std::false_type
{};

template<typename Component>
struct is_component_dependent<Component, detail::is_dependent_sfinae<Component>>
    : std::true_type
{};

template<typename Component>
constexpr bool is_component_dependent_v =
    is_component_dependent<Component>::value;

template<typename Component, typename = void>
struct component_depends_on
{
    using type = std::tuple<typename Component::depends_on>;
};

template<typename Component>
struct component_depends_on<
    Component,
    std::void_t<std::enable_if_t<
        detail::is_specialization_of<typename Component::depends_on,
                                     std::tuple>::value>>>
{
    using type = typename Component::depends_on;
};

template<typename Component>
using component_depends_on_t = typename component_depends_on<Component>::type;

template<typename Component, typename = void>
struct is_component : std::false_type
{};

template<typename Component>
struct is_component<Component, detail::is_component_sfinae<Component>>
    : std::true_type
{};

template<typename Component>
constexpr bool is_component_v = is_component<Component>::value;

template<typename Component, typename... Cs>
struct is_component_depends_present
    : matter::detail::tuple_in_list<component_depends_on_t<Component>, Cs...>
{};

template<typename Component, typename... Cs>
constexpr bool is_component_depends_present_v =
    is_component_depends_present<Component, Cs...>::value;

template<typename Component, typename = void>
struct is_component_variant : std::false_type
{};

template<typename Component>
struct is_component_variant<Component, detail::is_variant_sfinae<Component>>
    : std::true_type
{};

template<typename Component>
constexpr bool is_component_variant_v = is_component_variant<Component>::value;

} // namespace matter

#endif
