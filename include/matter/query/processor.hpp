#ifndef MATTER_QUERY_PROCESSOR_HPP
#define MATTER_QUERY_PROCESSOR_HPP

#pragma once

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/type.hpp>
#include <range/v3/view/transform.hpp>

#include "matter/id/id_cache.hpp"
#include "matter/query/primitives/filter.hpp"
#include "matter/query/type_traits.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename EntityQuery, typename Range, typename Identifier>
constexpr decltype(auto) process_entity_query(EntityQuery&      eq,
                                              Range&&           rng,
                                              const Identifier& ident) noexcept
{
    static_assert(matter::traits::is_entity_query(boost::hana::typeid_(eq)));

    auto query_primitives = decltype(matter::traits::to_hana_tuple_t(
        std::declval<
            matter::meta::tuplify_t<typename EntityQuery::query_types>>())){};

    return boost::hana::unpack(query_primitives, [&](auto... query_types) {
        auto transformed_range =
            std::forward<Range>(rng) |
            ranges::view::transform(
                [         =,
                 id_cache = matter::id_cache{
                     ident,
                     boost::hana::type_c<typename decltype(
                         query_types)::type::element_type>...}](auto grp) {
                    return matter::filter_group(grp, id_cache, query_types...);
                });

        return eq(std::move(transformed_range));
    });
}

template<typename Query, typename World>
constexpr decltype(auto) process_query(Query& q, World& w) noexcept
{
    constexpr auto query_type = boost::hana::typeid_(q);

    if constexpr (matter::traits::is_entity_query(query_type))
    {
        static_assert(
            matter::is_component_identifier_v<World>,
            "World for entity query must be a valid ComponentIdentifier");

        return process_entity_query(q, w.group_range(), w);
    }
    // TODO: group query, meta information will contain handle to the group
    // TODO: group filter query, no metainformation will be passed
    // TODO: world query, no metainformation will be passed
}

namespace traits
{
template<typename Query, typename World>
constexpr auto process_query_result(boost::hana::basic_type<Query>,
                                    boost::hana::basic_type<World>) noexcept
{
    using boost::hana::type_c;

    using process_query_result_type =
        decltype(process_query(std::declval<Query&>(), std::declval<World&>()));

    return type_c<process_query_result_type>;
}
} // namespace traits
} // namespace matter

#endif
