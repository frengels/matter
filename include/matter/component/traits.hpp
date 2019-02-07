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
using is_dependent_sfinae = std::void_t<typename Component::dependent_on>;
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
    : std::false_type
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
} // namespace matter

#endif
