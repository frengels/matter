#ifndef MATTER_QUERY_TYPE_TRAITS_HPP
#define MATTER_QUERY_TYPE_TRAITS_HPP

#pragma once

#include <boost/hana/type.hpp>

#include "matter/query/category.hpp"

namespace matter
{
namespace traits
{
template<typename T>
constexpr bool is_entity_query(boost::hana::basic_type<T> eq_type) noexcept
{
    using boost::hana::type_c;

    if constexpr (!has_query_category(eq_type))
    {
        return false;
    }
    else
    {
        auto eq_category = type_c<matter::entity_query_tag>;
        return eq_category == query_category(eq_type);
    }
}
} // namespace traits
} // namespace matter

#endif
