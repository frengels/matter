#ifndef MATTER_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_TRAITS_HPP

#pragma once

#include <tuple>
#include <type_traits>
#include <vector>

#include "matter/util/meta.hpp"

namespace matter
{
namespace detail
{
template<typename Component>
using is_storage_defined_sfinae = std::void_t<typename Component::storage_type>;

template<typename Component>
using is_dependent_sfinae = std::void_t<typename Component::depends_on>;

template<typename Component>
using is_variant_sfinae = std::void_t<typename Component::variant_of>;

template<typename Component>
using is_named_sfinae = std::void_t<std::enable_if_t<
    std::is_constructible_v<std::string_view, decltype(Component::name)>>>;
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
        !detail::is_specialization_of<Component, std::tuple>::value &&
        (std::is_nothrow_copy_constructible_v<Component> ||
         std::is_nothrow_move_constructible_v<Component>) )>> : std::true_type
{};

template<typename Component>
constexpr bool is_component_v = is_component<Component>::value;

/// \brief detects empty components
/// empty components are typically used for something like a tag, classes which
/// qualify for this constraint will be optimized to not take up any space in
/// storage.
template<typename Component>
struct is_component_empty : std::is_empty<Component>
{};

template<typename Component>
constexpr bool is_component_empty_v = is_component_empty<Component>::value;

template<typename Component, typename = void>
struct is_component_storage_defined : std::false_type
{};

/// \brief whether the component has custom storage defined
/// components with `storage_type` defined will use this instead of the default
/// predefined storage
template<typename Component>
struct is_component_storage_defined<
    Component,
    detail::is_storage_defined_sfinae<Component>> : std::true_type
{};

template<typename Component>
constexpr bool is_component_storage_defined_v =
    is_component_storage_defined<Component>::value;

template<typename Component, typename = void>
struct is_component_dependent : std::false_type
{};

/// \brief This component depends on another component's presence
/// If this component is assigned to an entity then all components specified as
/// dependency must be present in the entity. This dependency is checked for at
/// runtime. Dependencies can be specified with `depends_on`.
/// It is used for a now obsolete optimization.
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

/// \brief Get all of this components dependencies.
/// Returns all dependencies of this component as a tuple.
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

/// \brief Check whether dependencies exist
/// Holds true if all dependencies of the current component are present in the
/// given component list.
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

/// \brief This component is a variant of another
/// A variant is a variation of a specific tag component. There can only be one
/// variant of each tag present within the entity.
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

/// \brief lists all variants of this tag
/// Will find all possible variants of the given tag and return them as a tuple
/// of these variants.
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

template<typename Component, typename = void>
struct is_component_named : std::false_type
{};

template<typename Component>
struct is_component_named<Component,
                          std::void_t<detail::is_named_sfinae<Component>>>
    : is_component<Component>
{};

template<typename Component>
constexpr bool is_component_named_v = is_component_named<Component>::value;

template<typename Component, typename = void>
struct component_name;

template<typename Component>
struct component_name<Component,
                      std::enable_if_t<is_component_named_v<Component>>>
{
    using value_type                  = std::string_view;
    static constexpr value_type value = std::string_view(Component::name);

    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

template<typename Component>
constexpr auto component_name_v = component_name<Component>::value;

template<typename Component, typename = void>
struct component_storage
{
    using type = std::vector<Component>;
};

template<typename Component>
struct component_storage<
    Component,
    std::enable_if_t<is_component_storage_defined_v<Component>>>
{
    using type = typename Component::storage_type;
};

template<typename Component>
using component_storage_t = typename component_storage<Component>::type;
} // namespace matter

#endif
