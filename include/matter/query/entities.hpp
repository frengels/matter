#ifndef MATTER_QUERY_ENTITIES_HPP
#define MATTER_QUERY_ENTITIES_HPP

#pragma once

#include "matter/query/category.hpp"
#include "matter/query/type_query.hpp"
#include "matter/util/filter_transform.hpp"

namespace matter
{
template<typename... TypeAccess>
class entities;

template<typename... Ts, typename... Access, typename... Presence>
class entities<matter::type_query<Ts, Access, Presence>...> {
public:
    using query_category = matter::entity_query_tag;
    using query_types = std::tuple<matter::type_query<Ts, Access, Presence>...>;

public:
    constexpr entities(boost::hana::basic_type<
                       matter::type_query<Ts, Access, Presence>>...) noexcept
    {}

    /// normally we receive a range of all filtered groups as an optional. All
    /// this function does it only return the unwrapped optionals which hold a
    /// value.
    /// turning `[some(a), some(b), none, some(c)]` into `[a, b, c]`.
    template<typename Range>
    constexpr decltype(auto) operator()(Range&& group_rng) const noexcept
    {
        return std::forward<Range>(group_rng) |
               filter_transform([](auto&& opt_result) {
                   return std::forward<decltype(opt_result)>(opt_result);
               });
    }
};

template<typename... Ts, typename... Access, typename... Presence>
entities(boost::hana::basic_type<
         matter::type_query<Ts, Access, Presence>>...) noexcept
    ->entities<matter::type_query<Ts, Access, Presence>...>;
} // namespace matter

#endif
