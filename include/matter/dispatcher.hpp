#ifndef MATTER_DISPATCHER_HPP
#define MATTER_DISPATCHER_HPP

#pragma once

#include <tuple>

namespace matter
{
template<typename World, typename... Systems>
class dispatcher {
private:
    World*                 world_;
    std::tuple<Systems...> systems_;

public:
    constexpr dispatcher(World& w, Systems&&... sys) noexcept
        : world_{std::addressof(w)}, systems_{std::move(sys)...}
    {}

    constexpr void operator()() noexcept
    {
        std::apply([&](auto&&... systems) { (systems(*world_), ...); },
                   systems_);
    }
};
} // namespace matter

#endif
