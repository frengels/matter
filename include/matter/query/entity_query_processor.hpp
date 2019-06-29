#ifndef MATTER_QUERY_ENTITY_QUERY_PROCESSOR_HPP
#define MATTER_QUERY_ENTITY_QUERY_PROCESSOR_HPP

#pragma once

#include <boost/hana/tuple.hpp>
#include <range/v3/view/transform.hpp>

#include "matter/id/id_cache.hpp"
#include "matter/query/primitives/filter.hpp"
#include "matter/query/typed_access.hpp"

namespace matter
{
class entity_query_processor {
public:
    // returns a range of optional query results, When the query passed the
    // optional will be filled, otherwise it will be empty.
    template<typename Identifier,
             typename Range,
             typename... Ts,
             typename... Access,
             typename... Presence>
    constexpr decltype(auto) operator()(
        Range&&           group_range,
        const Identifier& ident,
        boost::hana::basic_type<
            matter::typed_access<Ts, Access, Presence>>... query_types) const
        noexcept
    {
        // use the filter_group function to determine whether the group is
        // valid. receive an empty optional if not.
        return ranges::transform_view(
            std::forward<Range>(group_range),
            [         =,
             id_cache = matter::id_cache{
                 ident,
                 boost::hana::type_c<typename decltype(
                     query_types)::type::element_type>...}](auto grp) {
                return matter::filter_group(grp, id_cache, query_types...);
            });
    }
};
} // namespace matter

#endif
