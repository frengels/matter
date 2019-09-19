#pragma once

#include <iterator>
#include <utility>

#include "matter/utility/decay_copy.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace begin_impl
{
template<typename R>
void begin(R&&) = delete;

template<typename T>
void begin(std::initializer_list<T>&&) = delete;

template<typename R>
constexpr auto begin(priority_tag<2>,
                     R& range) noexcept(noexcept(decay_copy(begin(range))))
    -> decltype(decay_copy(begin(range)))
{
    return decay_copy(begin(range));
}

template<typename R>
constexpr auto begin(priority_tag<3>,
                     R& range) noexcept(noexcept(decay_copy(range.begin())))
    -> decltype(decay_copy(range.begin()))
{
    return decay_copy(range.begin());
}

// implements: http://eel.is/c++draft/range.access.begin#1.1
template<typename T, std::size_t N>
constexpr auto begin(priority_tag<4>, T (&arr)[N]) noexcept(noexcept(arr + 0))
    -> decltype(arr + 0)
{
    return arr + 0;
}
} // namespace begin_impl

inline namespace cpo
{
struct begin_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(begin_impl::begin(max_priority_tag,
                                            std::forward<R>(r))))
            -> decltype(begin_impl::begin(max_priority_tag, std::forward<R>(r)))
    {
        return begin_impl::begin(max_priority_tag, std::forward<R>(r));
    }
};

constexpr auto begin = begin_fn{};
} // namespace cpo
} // namespace matter
