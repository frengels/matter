#ifndef MATTER_QUERY_PRIMITIVES_WRITE_HPP
#define MATTER_QUERY_PRIMITIVES_WRITE_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/query/runtime.hpp"
#include "matter/util/concepts.hpp"

namespace matter
{
namespace prim
{
struct write
{
    struct storage_modifier
    {
        template<typename T>
        constexpr std::enable_if_t<matter::is_optional_v<T>, T>
        operator()(T store) const noexcept
        {
            return store;
        }
    };

    static constexpr matter::access access_enum() noexcept
    {
        return matter::access::write;
    }
};
} // namespace prim
} // namespace matter

#endif
