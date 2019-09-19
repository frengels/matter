#pragma once

#include <cstddef>

namespace matter
{
template<std::size_t P>
struct priority_tag : priority_tag<P - 1>
{};

template<>
struct priority_tag<0>
{};

constexpr auto max_priority_tag = priority_tag<4>{};
} // namespace matter
