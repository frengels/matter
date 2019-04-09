#ifndef MATTER_UTIL_CONCEPTS_HPP
#define MATTER_UTIL_CONCEPTS_HPP

#pragma once

#include <type_traits>

namespace matter
{
template<typename T, typename U, typename = void>
struct has_less_than : std::false_type
{};

template<typename T, typename U>
struct has_less_than<
    T,
    U,
    std::void_t<decltype(std::declval<const T&>() < std::declval<const U&>())>>
    : std::true_type
{};

template<typename T, typename U, typename = void>
struct has_minus : std::false_type
{};

template<typename T, typename U>
struct has_minus<
    T,
    U,
    std::void_t<decltype(std::declval<const T&>() - std::declval<const U&>())>>
    : std::true_type
{};

template<typename T, typename U, typename = void>
struct has_add : std::false_type
{};

template<typename T, typename U>
struct has_add<
    T,
    U,
    std::void_t<decltype(std::declval<const T&>() + std::declval<const U&>())>>
    : std::true_type
{};

template<typename T, typename = void>
struct has_pre_increment : std::false_type
{};

template<typename T>
struct has_pre_increment<T, std::void_t<decltype(++std::declval<T&>())>>
    : std::true_type
{};
} // namespace matter

#endif
