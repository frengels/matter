#ifndef MATTER_QUERY_PRIMITIVES_OPTIONAL_HPP
#define MATTER_QUERY_PRIMITIVES_OPTIONAL_HPP

#pragma once

#include <optional>

#include "matter/query/runtime.hpp"

namespace matter
{
namespace prim
{
struct optional
{
    struct storage_filter
    {
        template<typename Storage>
        constexpr std::optional<Storage> operator()(Storage store) const
            noexcept
        {
            return store;
        }
    };

    static constexpr matter::presence presence_enum() noexcept
    {
        return presence::optional;
    }
};
} // namespace prim
} // namespace matter

#endif
