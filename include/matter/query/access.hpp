#ifndef MATTER_QUERY_ACCESS_HPP
#define MATTER_QUERY_ACCESS_HPP

#pragma once

#include <type_traits>

#include "matter/component/prototype.hpp"
#include "matter/query/runtime.hpp"
#include "matter/util/concepts.hpp"

namespace matter
{
template<typename T, typename = void>
struct is_access : std::false_type
{};

/// Access specifiers can be seen as modifiers to the base type of storage. This
/// is done so that we can safely evaluate whether concurrent access is possible
/// or not.
template<typename T>
struct is_access<
    T,
    std::enable_if_t<
        std::is_same_v<matter::access, decltype(T::access_enum())> &&
            matter::is_optional_v<decltype(
                std::declval<const typename T::storage_modifier>()(
                    std::declval<matter::component_storage_t<
                        matter::prototype::component>*>()))> &&
            std::is_default_constructible_v<typename T::storage_modifier>,
        std::void_t<typename T::storage_modifier>>> : std::true_type
{};

template<typename T>
constexpr bool is_access_v = is_access<T>::value;
} // namespace matter

#endif
