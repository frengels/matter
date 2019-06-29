#ifndef MATTER_QUERY_PRIMITIVES_EXCLUDE_HPP
#define MATTER_QUERY_PRIMITIVES_EXCLUDE_HPP

#pragma once

#include <optional>

#include "matter/query/runtime.hpp"
#include "matter/util/empty.hpp"

namespace matter
{
namespace prim
{
struct exclude
{
    struct storage_filter
    {
        template<typename Storage>
        constexpr std::optional<matter::empty>
        operator()(Storage store) noexcept
        {
            if (store)
            {
                return std::nullopt;
            }
            else
            {
                return matter::empty{};
            }
        }
    };

    static constexpr matter::presence presence_enum() noexcept
    {
        return presence::exclude;
    }
};
} // namespace prim
} // namespace matter

#endif
