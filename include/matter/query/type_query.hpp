#ifndef MATTER_QUERY_TYPE_QUERY_HPP
#define MATTER_QUERY_TYPE_QUERY_HPP

#pragma once

#include <boost/hana/type.hpp>

#include "matter/query/access.hpp"
#include "matter/query/presence.hpp"
#include "matter/query/primitives/exclude.hpp"
#include "matter/query/primitives/inaccessible.hpp"
#include "matter/query/primitives/optional.hpp"
#include "matter/query/primitives/read.hpp"
#include "matter/query/primitives/require.hpp"
#include "matter/query/primitives/write.hpp"

namespace matter
{
template<typename T, typename Access, typename Presence>
class type_query {
    static_assert(matter::is_access_v<Access>);
    static_assert(matter::is_presence_v<Presence>);

public:
    using element_type = T;

    using access_type   = Access;
    using presence_type = Presence;

    static constexpr matter::access access_enum() noexcept
    {
        return access_type::access_enum();
    }

    static constexpr matter::presence presence_enum() noexcept
    {
        return presence_type::presence_enum();
    }
};

template<typename T>
using read = matter::type_query<T, matter::prim::read, matter::prim::require>;

template<typename T>
using write = matter::type_query<T, matter::prim::write, matter::prim::require>;

template<typename T>
using opt_read =
    matter::type_query<T, matter::prim::read, matter::prim::optional>;

template<typename T>
using opt_write =
    matter::type_query<T, matter::prim::write, matter::prim::optional>;

template<typename T>
using exclude =
    matter::type_query<T, matter::prim::inaccessible, matter::prim::exclude>;
} // namespace matter

#endif
