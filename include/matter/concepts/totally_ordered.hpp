#pragma once

#include "matter/concepts/boolean.hpp"
#include "matter/concepts/equality_comparable.hpp"

namespace matter
{
template<typename T>
concept totally_ordered = // clang-format off
    equality_comparable<T> &&
    requires(const std::remove_reference_t<T>& a,
             const std::remove_reference_t<T>& b)
    {
        { a <  b } -> boolean;
        { a >  b } -> boolean;
        { a <= b } -> boolean;
        { a >= b } -> boolean;
    }; // clang-format on
} // namespace matter