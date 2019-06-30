#ifndef MATTER_QUERY_CATEGORY_HPP
#define MATTER_QUERY_CATEGORY_HPP

#pragma once

#include <type_traits>

#include <boost/hana/type.hpp>

namespace matter
{
struct entity_query_tag
{};

template<typename QueryCategory>
struct is_query_category : std::false_type
{};

template<>
struct is_query_category<entity_query_tag> : std::true_type
{};

namespace traits
{
template<typename T>
constexpr bool
is_query_category(boost::hana::basic_type<T> maybe_query_cat = {}) noexcept
{
    using boost::hana::type_c;

    if (maybe_query_cat == type_c<entity_query_tag>)
    {
        return true;
    }

    return false;
}

constexpr auto has_query_category = boost::hana::is_valid(
    []<typename T>(boost::hana::basic_type<T>)
        ->boost::hana::type<typename T::query_category>{});

template<typename T>
constexpr auto query_category(boost::hana::basic_type<T>) noexcept
{
    return boost::hana::type_c<typename T::query_category>;
}

} // namespace traits
} // namespace matter

#endif
