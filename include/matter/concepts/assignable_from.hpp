#pragma once

#include <utility>

#include "matter/concepts/common_reference_with.hpp"
#include "matter/concepts/same_as.hpp"

namespace matter
{
template<typename LHS, typename RHS>
concept assignable_from = std::is_lvalue_reference_v<LHS>&&
    common_reference_with<
        const std::remove_reference_t<LHS>&,
        const std::remove_reference_t<RHS>&>&& // clang-format off
    requires(LHS lhs, RHS&& rhs) {
        { lhs = std::forward<RHS>(rhs) };
        requires same_as<decltype(lhs = std::forward<RHS>(rhs)), LHS>;
    }; // clang-format on
} // namespace matter