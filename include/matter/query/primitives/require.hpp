#ifndef MATTER_QUERY_PRIMITIVES_REQUIRE_HPP
#define MATTER_QUERY_PRIMITIVES_REQUIRE_HPP

#pragma once

#include "matter/query/runtime.hpp"

namespace matter
{
namespace prim
{
struct require
{
    struct storage_filter
    {
        template<typename Storage>
        constexpr Storage operator()(Storage store) const noexcept
        {
            return store;
        }
    };

    static constexpr matter::presence presence_enum() noexcept
    {
        return presence::require;
    }
};
} // namespace prim
} // namespace matter

#endif
