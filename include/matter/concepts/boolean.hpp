#pragma once

#include "matter/concepts/convertible_to.hpp"
#include "matter/concepts/movable.hpp"

namespace matter
{
template<typename B>
concept boolean = // clang-format off
    movable<std::remove_cvref_t<B>> &&
    requires(const std::remove_reference_t<B>& b1,
             const std::remove_reference_t<B>& b2, const bool a)
    {
        {  b1 } -> convertible_to<bool>;
        { !b1 } -> convertible_to<bool>;
        { b1 && b2 } -> convertible_to<bool>;
        { b1 &&  a } -> convertible_to<bool>;
        {  a && b2 } -> convertible_to<bool>;
        { b1 || b2 } -> convertible_to<bool>;
        { b1 ||  a } -> convertible_to<bool>;
        {  a || b2 } -> convertible_to<bool>;
        { b1 == b2 } -> convertible_to<bool>;
        { b1 ==  a } -> convertible_to<bool>;
        {  a == b2 } -> convertible_to<bool>;
        { b1 != b2 } -> convertible_to<bool>;
        { b1 !=  a } -> convertible_to<bool>;
        {  a != b2 } -> convertible_to<bool>;
    }; // clang-format on
} // namespace matter