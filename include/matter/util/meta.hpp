#ifndef MATTER_UTIL_META_HPP
#define MATTER_UTIL_META_HPP

#pragma once

#include <type_traits>

namespace matter
{
namespace detail
{
template<typename T1, typename T2, typename U = void>
using enable_if_same_t = std::enable_if_t<std::is_same_v<T1, T2>, U>;

template<typename, template<typename...> typename>
struct is_specialization_of : std::false_type
{};

template<template<typename...> typename TTemplate, typename... Ts>
struct is_specialization_of<TTemplate<Ts...>, TTemplate> : std::true_type
{};
} // namespace detail
} // namespace matter

#endif
