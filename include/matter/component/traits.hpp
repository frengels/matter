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

template<typename Component, typename = void>
struct is_component : std::false_type
{};

/// \brief Detects whether a struct is a valid component
/// Components must be copy constructible/assignable, and either trivially
/// copyable/assignable or nothrow move constructible/assignable
template<typename Component>
struct is_component<
    Component,
    std::enable_if_t<(
        std::is_nothrow_copy_constructible_v<Component> &&
        std::is_nothrow_copy_assignable_v<
            Component>) &&((std::is_trivially_copyable_v<Component> &&
                            std::is_trivially_assignable_v<Component>) ||
                           (std::is_nothrow_move_constructible_v<Component> &&
                            std::is_nothrow_move_assignable_v<Component>) )>>
    : std::true_type
{};

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

template<typename Candidate, typename Component, typename = void>
struct is_component_variant_of : std::false_type
{};

template<typename Candidate, typename Component>
struct is_component_variant_of<Candidate,
                               Component,
                               detail::is_variant_sfinae<Candidate>>
    : std::is_same<typename Candidate::variant_of, Component>
{};

template<typename Candidate, typename Component>
constexpr bool is_component_variant_of_v =
    is_component_variant_of<Candidate, Component>::value;

namespace detail
{
/// \brief void if the Candidate is not a variant of the Component
/// Used for merging using `merge_non_void`, which merges all types that aren't
/// void, and that's why if a Candidate isn't a variant of Component we want to
/// return void.
template<typename Candidate, typename Component>
struct if_not_variant_of_void;
} // namespace detail

template<typename Component, typename... Cs>
struct component_variants
    : detail::merge_non_void<
          std::conditional_t<is_component_variant_of_v<Cs, Component>,
                             Cs,
                             void>...>
{};

template<typename Component, typename... Cs>
using component_variants_t =
    typename component_variants<Component, Cs...>::type;
} // namespace matter

#endif
