#ifndef MATTER_UTIL_FILTER_TRANSFORM_HPP
#define MATTER_UTIL_FILTER_TRANSFORM_HPP

#pragma once

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

namespace matter
{
/// A convenience for filtering and transforming at the same time, the predicate
/// must return the transformed element as an optional (pointer or std::optional
/// works). If the optional evaluates to true it will be dereferenced and
/// accessible in the resulting range, otherwise it will be discarded.
template<typename Pred>
constexpr decltype(auto) filter_transform(Pred p)
{
    return ranges::view::transform([p = std::move(p)](auto&& element) {
               return p(std::forward<decltype(element)>(element));
           }) |
           ranges::view::filter([](auto&& opt_element) -> bool {
               return opt_element.has_value();
           }) |
           ranges::view::transform([](auto&& opt_element) {
               return *std::forward<decltype(opt_element)>(opt_element);
           });
}

template<typename Rng, typename Pred>
constexpr decltype(auto) filter_transform_view(Rng&& range, Pred p)
{
    return std::forward<Rng>(range) | filter_transform(std::move(p));
}
} // namespace matter

#endif
