#ifndef MATTER_QUERY_COMPONENT_TYPED_ACCESS_HPP
#define MATTER_QUERY_COMPONENT_TYPED_ACCESS_HPP

#pragma once

#include "matter/query/access.hpp"
#include "matter/query/presence.hpp"

namespace matter
{
template<typename T, typename Access, typename Presence>
class typed_access {
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
} // namespace matter

#endif
