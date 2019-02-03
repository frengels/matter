#ifndef MATTER_STORAGE_TRAITS_HPP
#define MATTER_STORAGE_TRAITS_HPP

#pragma once

#include <type_traits>

#include "matter/util/meta.hpp"

namespace matter
{
namespace detail
{
template<typename Storage, typename = void>
struct storage_types
{};

template<typename Storage>
struct storage_types<
    Storage,
    std::void_t<typename Storage::id_type,
                typename Storage::value_type,
                matter::detail::enable_if_same_t<
                    void,
                    decltype(std::declval<Storage&>().emplace(
                        std::declval<typename Storage::value_type&&>()))>,
                matter::detail::enable_if_same_t<
                    void,
                    decltype(std::declval<Storage&>().erase(
                        std::declval<typename Storage::id_type>()))>,
                matter::detail::enable_if_same_t<
                    bool,
                    decltype(std::declval<Storage&>().has(
                        std::declval<typename Storage::id_type>()))>>>
{
    using value_type = typename Storage::value_type;
};
} // namespace detail

template<typename Storage>
struct storage_traits : detail::storage_types<Storage>
{};
} // namespace matter

#endif
