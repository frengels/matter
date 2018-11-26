#ifndef MATTER_COMPONENT_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_COMPONENT_TRAITS_HPP

#pragma once

#include <type_traits>

namespace matter::traits
{
/**
 * The component_traits struct should be specialized for each type of component
 * you wish to use, provided properties should be:
 *  - storage_type: required storage type
 *  - meta_type [optional]: meta information for serialization
 */
template<typename C>
struct component_traits;
} // namespace matter::traits

#endif
