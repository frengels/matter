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

namespace impl
{
template<std::size_t I, typename T, typename... Ts>
struct index_of_impl : index_of_impl<I + 1, Ts...>
{};

template<std::size_t I, typename T, typename... Ts>
struct index_of_impl<I, T, T, Ts...> : std::integral_constant<std::size_t, I>
{};
} // namespace impl

template<typename T, typename... Ts>
struct index_of : impl::index_of_impl<0, T, Ts...>
{
    static_assert(contains<T, Ts...>::value,
                  "Type pack Ts... must at least contain T.");
};

} // namespace detail
} // namespace matter

#endif
