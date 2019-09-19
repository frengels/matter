#pragma once

#include <iterator>
#include <utility>

#include "matter/utility/decay_copy.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace end_impl
{
template<typename R>
constexpr auto end(R&&) = delete;

template<typename T>
constexpr auto end(std::initializer_list<T>&&) = delete;

template<typename R>
constexpr auto end(priority_tag<2>,
                   R& range) noexcept(noexcept(decay_copy(end(range))))
    -> decltype(decay_copy(end(range)))
{
    return decay_copy(end(range));
}

template<typename R>
constexpr auto end(priority_tag<3>,
                   R& range) noexcept(noexcept(decay_copy(range.end())))
    -> decltype(decay_copy(range.end()))
{
    return decay_copy(range.end());
}

template<typename T, std::size_t N>
constexpr auto end(priority_tag<4>, T (&arr)[N]) noexcept(noexcept(arr + N))
    -> decltype(arr + N)
{
    return arr + N;
}
} // namespace end_impl

inline namespace cpo
{
struct end_fn
{
    template<typename R>
    constexpr auto operator()(R&& r) const
        noexcept(noexcept(end_impl::end(max_priority_tag, std::forward<R>(r))))
            -> decltype(end_impl::end(max_priority_tag, std::forward<R>(r)))
    {
        return end_impl::end(max_priority_tag, std::forward<R>(r));
    }
};

constexpr auto end = end_fn{};
} // namespace cpo
} // namespace matter
