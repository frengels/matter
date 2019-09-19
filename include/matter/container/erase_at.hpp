#pragma once

#include <utility>

#include "matter/iterator/begin.hpp"
#include "matter/iterator/concepts.hpp"
#include "matter/ranges/traits.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace erase_at_impl
{
template<typename C>
constexpr auto erase_at(priority_tag<4>, C& c, iterator_t<C> pos) noexcept(
    noexcept(c.erase(std::move(pos)))) -> decltype(c.erase(std::move(pos)))
{
    return c.erase(std::move(pos));
}

template<typename C> // clang-format off
    requires sized_sentinel_for<iterator_t<C>, iterator_t<C>> // clang-format on
    constexpr auto erase_at(priority_tag<3>, C& c, iterator_t<C> pos) noexcept(
        noexcept(c.erase(c,
                         static_cast<range_size_t<C>>(pos - matter::begin(c)))))
        -> decltype(
            c.erase(c, static_cast<range_size_t<C>>(pos - matter::begin(c))))
{
    return c.erase(c, static_cast<range_size_t<C>>(pos - matter::begin(c)));
}
} // namespace erase_at_impl

inline namespace cpo
{
struct erase_at_fn
{
    template<typename C>
    constexpr auto operator()(C& c, iterator_t<C> pos) const noexcept(noexcept(
        ::matter::erase_at_impl::erase_at(max_priority_tag, c, std::move(pos))))
        -> decltype(::matter::erase_at_impl::erase_at(max_priority_tag,
                                                      c,
                                                      std::move(pos)))
    {
        return ::matter::erase_at_impl::erase_at(
            max_priority_tag, c, std::move(pos));
    }
};

// first tries to call .erase(it).
// otherwise tries call .erase(it - begin) (attempt index erase) if the iterator
// models sized_sentinel_for.
constexpr auto erase_at = erase_at_fn{};
} // namespace cpo
} // namespace matter