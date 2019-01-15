#ifndef MATTER_COMPONENT_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_COMPONENT_TRAITS_HPP

#pragma once

#include <type_traits>

namespace matter
{
/**
 * The component_traits struct should be specialized for each type of component
 * you wish to use, provided properties should be:
 *  - storage_type: required storage type
 *  - meta_type [optional]: meta information for serialization
 */
template<typename C>
struct component_traits;

template<typename C, typename = void>
struct has_storage_type : std::false_type
{};

template<typename C>
struct has_storage_type<C,
                        std::void_t<typename component_traits<C>::storage_type>>
    : std::true_type
{};
} // namespace matter

#endif
