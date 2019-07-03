#ifndef MATTER_SYSTEM_SYSTEM_HPP
#define MATTER_SYSTEM_SYSTEM_HPP

#pragma once

#include <tuple>

#include "matter/system/job.hpp"

namespace matter
{
template<typename... Jobs>
class system {
private:
    std::tuple<Jobs...> jobs_;

public:
    constexpr system(Jobs&&... js) noexcept : jobs_{std::move(js)...}
    {}

    template<typename World>
    constexpr void operator()(World& w)
    {
        static_assert((matter::JobFor<Jobs, World> && ...),
                      "Job is incompatible with World");
    }
};
} // namespace matter

#endif
