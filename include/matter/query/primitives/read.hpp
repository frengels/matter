#ifndef MATTER_QUERY_PRIMITIVES_READ_HPP
#define MATTER_QUERY_PRIMITIVES_READ_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/query/runtime.hpp"
#include "matter/util/concepts.hpp"

namespace matter
{
namespace prim
{
struct read
{
    struct storage_modifier
    {
        template<typename T>
        constexpr std::enable_if_t<matter::is_optional_v<T>, const T>
        operator()(T store) const noexcept
        {
            return store;
        }
    };

    static constexpr matter::access access_enum() noexcept
    {
        return matter::access::read;
    }
};
} // namespace prim
} // namespace matter

#endif
