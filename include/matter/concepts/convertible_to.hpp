#pragma once

#include <type_traits>

namespace matter
{
template<typename From, typename To>
concept convertible_to = // clang-format off
    std::is_convertible_v<From, To>&& 
    requires(From (&f)())
    {
        static_cast<To>(f());
    }; // clang-format on
} // namespace matter