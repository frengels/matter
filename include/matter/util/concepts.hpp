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

template<typename T, typename It, typename = void>
struct has_erase_for : std::false_type
{};

template<typename T, typename It>
struct has_erase_for<
    T,
    It,
    std::void_t<decltype(std::declval<T&>().erase(std::declval<It>()))>>
    : std::true_type
{};

template<typename T, typename = void>
struct is_optional : std::false_type
{};

template<typename T>
struct is_optional<T,
                   std::void_t<decltype(bool(std::declval<const T>())),
                               decltype(*std::declval<T>())>> : std::true_type
{};

template<typename T>
constexpr bool is_optional_v = is_optional<T>::value;

template<typename T, typename = void>
struct is_swappable : std::false_type
{};

template<typename T>
struct is_swappable<
    T,
    std::void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::true_type
{};

template<typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;
} // namespace matter

#endif
