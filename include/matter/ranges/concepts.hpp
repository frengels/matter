#pragma once

#include <utility>

#include "matter/concepts/same_as.hpp"
#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/iterator/traits.hpp"
#include "matter/ranges/size.hpp"

namespace matter
{
namespace detail
{
template<typename R>
concept range_impl = // clang-format off
    requires(R&& r)
    {
        matter::begin(std::forward<R>(r));
        matter::end(std::forward<R>(r));
    }; // clang-format on
} // namespace detail

template<typename R>
concept range = detail::range_impl<R&>;

template<typename R>
concept sized_range = // clang-format off
    range<R>&& 
        requires(R& r)
        {
            matter::size(r);
        }; // clang-format on

template<typename R>
concept common_range = // clang-format off
    range<R> && same_as<iterator_t<R>, sentinel_t<R>>;
} // namespace matter