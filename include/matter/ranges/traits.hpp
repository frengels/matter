#pragma once

#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/iterator/traits.hpp"
#include "matter/ranges/size.hpp"

namespace matter
{
template<typename R>
using iterator_t = decltype(matter::begin(std::declval<R&>()));

template<typename R>
using sentinel_t = decltype(matter::end(std::declval<R&>()));

template<typename R>
using range_value_t = iter_value_t<iterator_t<R>>;

template<typename R>
using range_reference_t = iter_reference_t<iterator_t<R>>;

template<typename R>
using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;

template<typename R>
using range_size_t = decltype(matter::size(std::declval<R&>()));
} // namespace matter