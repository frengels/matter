#pragma once

namespace matter
{
namespace detail
{
template<typename T>
using with_reference = T&;
}

template<typename T>
concept can_reference = // clang-format off
    requires
    {
        typename detail::with_reference<T>;
    }; // clang-format on

template<typename T>
concept dereferenceable = // clang-format off
    requires (T& t) {
        { *t } -> can_reference;
    }; // clang-format on
} // namespace matter