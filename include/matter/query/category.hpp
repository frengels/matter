#ifndef MATTER_QUERY_CATEGORY_HPP
#define MATTER_QUERY_CATEGORY_HPP

#pragma once

#include <type_traits>

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
} // namespace matter

#endif
