#ifndef MATTER_UTIL_TYPE_LIST_HPP
#define MATTER_UTIL_TYPE_LIST_HPP

#pragma once

#include <type_traits>

namespace matter
{
template<typename T, typename... Ts>
struct has_type;

template<typename T, typename T1>
struct has_type<T, T1> : std::is_same<T, T1>
{};

template<typename T, typename T1, typename... Rest>
struct has_type<T, T1, Rest...> : std::conditional<std::is_same<T, T1>::value,
                                                   std::true_type,
                                                   has_type<T, Rest...>>
{};

} // namespace matter

#endif
