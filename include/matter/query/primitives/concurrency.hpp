#ifndef MATTER_QUERY_PRIMITIVES_CONCURRENCY_HPP
#define MATTER_QUERY_PRIMITIVES_CONCURRENCY_HPP

#pragma once

#include <boost/hana/type.hpp>

#include "matter/id/id.hpp"
#include "matter/id/typed_id.hpp"
#include "matter/query/component_query_description.hpp"
#include "matter/query/type_query.hpp"

namespace matter
{
namespace detail
{
// this identifier is used to unify the concurrency checking code at runtime and
// compile time.
template<typename T, typename U>
class mock_component_identifier {
public:
    using id_type = matter::signed_id<int>;

    template<typename TAccess,
             typename TPresence,
             typename UAccess,
             typename UPresence>
    constexpr mock_component_identifier(
        boost::hana::basic_type<matter::type_query<T, TAccess, TPresence>>,
        boost::hana::basic_type<
            matter::type_query<U, UAccess, UPresence>>) noexcept
    {}

    template<typename V>
    constexpr bool contains_component() const noexcept
    {
        if constexpr (std::is_same_v<V, T> || std::is_same_v<V, U>)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // this just gives the id of 0 for one and 1 for the other
    template<typename V>
    constexpr std::enable_if_t<std::is_same_v<V, T> || std::is_same_v<V, U>,
                               matter::typed_id<id_type, V>>
    component_id() const noexcept
    {
        if constexpr (std::is_same_v<V, T>)
        {
            return matter::typed_id<id_type, V>{matter::signed_id<int>{0}};
        }
        else // has to be U now
        {
            return matter::typed_id<id_type, V>{matter::signed_id<int>{1}};
        }
    }
};
} // namespace detail

template<typename T,
         typename TAccess,
         typename TPresence,
         typename U,
         typename UAccess,
         typename UPresence>
constexpr bool can_access_concurrent(
    boost::hana::basic_type<matter::type_query<T, TAccess, TPresence>> lhs,
    boost::hana::basic_type<matter::type_query<U, UAccess, UPresence>>
        rhs) noexcept
{
    constexpr auto ident = matter::detail::mock_component_identifier{lhs, rhs};

    constexpr auto lhs_descr = matter::component_query_description{ident, lhs};
    constexpr auto rhs_descr = matter::component_query_description{ident, rhs};

    return lhs_descr.can_access_concurrent(rhs_descr);
}
} // namespace matter

#endif
