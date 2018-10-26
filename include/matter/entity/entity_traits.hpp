#ifndef MATTER_ENTITY_TRAITS_HPP
#define MATTER_ENTITY_TRAITS_HPP

#pragma once

#include <type_traits>

namespace matter
{
template<typename T, typename = void>
struct is_entity : public std::false_type
{};

template<typename T>
struct is_entity<
    T,
    std::void_t<
        typename T::id_type,
        typename T::version_type,
        std::enable_if_t<
            std::is_convertible_v<const typename T::id_type&,
                                  decltype(std::declval<const T&>().id())>>,
        std::enable_if_t<std::is_convertible_v<
            const typename T::version_type&,
            decltype(std::declval<const T&>().version())>>,
        std::enable_if_t<std::is_same_v<bool,
                                        decltype(std::declval<const T&>() ==
                                                 std::declval<const T&>())>>,
        decltype(bool(std::declval<const T&>()))>> : public std::true_type
{};

template<typename T>
constexpr bool is_entity_v = is_entity<T>::value;
} // namespace matter

#endif
