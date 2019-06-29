#ifndef MATTER_QUERY_PRESENCE_HPP
#define MATTER_QUERY_PRESENCE_HPP

#pragma once

#include <type_traits>

#include "matter/query/runtime.hpp"
#include "matter/util/concepts.hpp"
#include "matter/util/prototype.hpp"

namespace matter
{
template<typename T, typename = void>
struct is_presence : std::false_type
{};

/// Presence specifiers can be seen as a sort of filter, this is the reason why
/// all return values must fulfill the Optional concept.
/// Additionally it gets passed the type modified by the access specifier, which
/// is also required to be optional. So the processing functor must work with
/// all types which fulfill the optional concept.
template<typename T>
struct is_presence<
    T,
    std::enable_if_t<
        matter::is_optional_v<
            std::invoke_result_t<typename T::storage_filter,
                                 matter::prototype::optional>> &&
            std::is_same_v<matter::presence, decltype(T::presence_enum())> &&
            std::is_default_constructible_v<typename T::storage_filter>,
        std::void_t<typename T::storage_filter>>> : std::true_type
{};

template<typename T>
constexpr bool is_presence_v = is_presence<T>::value;
} // namespace matter

#endif
