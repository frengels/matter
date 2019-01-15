#ifndef MATTER_COMPONENT_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_COMPONENT_TRAITS_HPP

#pragma once

#include <type_traits>

#include "matter/component/sparse_vector_storage.hpp"

namespace matter
{
/**
 * The component_traits struct should be specialized for each type of component
 * you wish to use, provided properties should be:
 *  - template<typename Entity> storage_type: required storage type
 *  - meta_type [optional]: meta information for serialization
 */
template<typename C>
struct component_traits;

template<typename C, typename = void>
struct has_storage_type : std::false_type
{};

/// determines whether the storage_type traits was implemented for this
/// particular component
template<typename Entity, typename C>
struct has_storage_type<
    C,
    std::void_t<typename component_traits<C>::template storage_type<Entity>>>
    : std::true_type
{};

/// gets the storage_type set in component_traits or defaults to
/// sparse_vector_storage<C>
template<typename Entity, typename C>
struct storage_type
{
    using type =
        std::conditional_t<has_storage_type<C>::value,
                           component_traits<C>::template storage_type<Entity>,
                           matter::sparse_vector_storage<Entity, C>>;
};

template<typename Entity, typename C>
using storage_type_t = typename storage_type<Entity, C>::type;
} // namespace matter

#endif
