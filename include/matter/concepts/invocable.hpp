#pragma once

#include <functional>
#include <utility>

namespace matter
{
template<typename F, typename... Args>
concept invocable = // clang-format off
    requires(F&& fn, Args&&... args)
    {
        std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
    }; // clang-format on
} // namespace matter