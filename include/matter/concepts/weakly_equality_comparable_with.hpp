#pragma once

#include "matter/concepts/boolean.hpp"

namespace matter
{
template<typename T, typename U>
concept weakly_equality_comparable_with = // clang-format off
    requires (const std::remove_reference_t<T>& t,
              const std::remove_reference_t<U>& u)
    {
        { t == u } -> boolean;
        { t != u } -> boolean;
        { u == t } -> boolean;
        { u != t } -> boolean;
    }; // clang-format on
} // namespace matter