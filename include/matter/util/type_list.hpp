#ifndef MATTER_UTIL_TYPE_LIST_HPP
#define MATTER_UTIL_TYPE_LIST_HPP

#pragma once

#include <type_traits>

namespace matter
{
namespace detail
{
template<typename T, typename... Ts>
struct contains : std::false_type
{};

template<typename T, typename... Rest>
struct contains<T, T, Rest...> : std::true_type
{};

template<typename T, typename T1, typename... Rest>
struct contains<T, T1, Rest...> : contains<T, Rest...>
{};
} // namespace detail
} // namespace matter

#endif
