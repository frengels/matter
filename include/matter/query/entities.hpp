#ifndef MATTER_QUERY_ENTITIES_HPP
#define MATTER_QUERY_ENTITIES_HPP

#pragma once

#include "matter/query/typed_access.hpp"
#include "matter/util/filter_transform.hpp"

namespace matter
{
template<typename... TypeAccess>
class entities;

template<typename... Ts, typename... Access, typename... Presence>
class entities<matter::typed_access<Ts, Access, Presence>...> {
public:
    constexpr entities(boost::hana::basic_type<
                       matter::typed_access<Ts, Access, Presence>>...) noexcept
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

    constexpr auto query_types() const noexcept
    {

        return boost::hana::tuple_t<
            matter::typed_access<Ts, Access, Presence>...>;
    }
};

template<typename... Ts, typename... Access, typename... Presence>
entities(boost::hana::basic_type<
         matter::typed_access<Ts, Access, Presence>>...) noexcept
    ->entities<matter::typed_access<Ts, Access, Presence>...>;
} // namespace matter

#endif
