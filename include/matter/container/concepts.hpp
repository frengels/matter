#pragma once

#include <utility>

#include <hera/ranges.hpp>

#include "matter/concepts/same_as.hpp"
#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/ranges/size.hpp"
#include "matter/ranges/traits.hpp"

namespace matter
{
template<typename Cont, typename... Args>
concept emplace_back_for = // clang-format off
    requires(Cont& c, Args&&... args){
        c.emplace_back(std::forward<Args>(args)...);
    }; // clang-format on

template<typename Cont, typename... Args>
concept emplace_for = // clang-format off
    requires(Cont& c, matter::iterator_t<Cont> p, Args&&... args)
    {
        c.emplace(p, std::forward<Args>(args)...);
    }; // clang-format on

template<typename Cont, typename T>
concept push_back_for = // clang-format off
    requires(Cont& c, T&& val)
    {
        c.push_back(std::forward<T>(val));
    }; // clang-format on

template<typename T, typename Cont>
concept push_back_into = push_back_for<Cont, T>;

template<typename Cont>
concept reserveable = // clang-format off
    requires(Cont& c, matter::range_size_t<Cont> sz)
    {
        c.reserve(sz);
    }; // clang-format on

template<typename Cont>
concept shrinkable = // clang-format off
    requires(Cont& c)
    {
        c.shrink_to_fit();
    }; // clang-format on

template<typename Cont>
concept eraseable = // clang-format off
    requires(Cont& c, iterator_t<Cont> it)
    {
        c.erase(it);
        requires same_as<decltype(c.erase(it)), iterator_t<Cont>>;
    }; // clang-format off
} // namespace matter