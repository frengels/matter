#pragma once

#include "matter/concepts/destructible.hpp"

namespace matter
{
template<typename T, typename... Args>
concept constructible_from =
    destructible<T>&& std::is_constructible_v<T, Args...>;
}