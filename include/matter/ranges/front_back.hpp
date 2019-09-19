#pragma once

#include <utility>

#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"
#include "matter/ranges/concepts.hpp"
#include "matter/ranges/size.hpp"
#include "matter/utility/priority_tag.hpp"

namespace matter
{
namespace front_impl
{
template<typename T, std::size_t N>
constexpr auto front(priority_tag<4>,
                     T (&arr)[N]) noexcept(noexcept(*(arr + 0)))
    -> decltype(*(arr + 0))
{
    return *(arr + 0);
}

template<typename R>
constexpr auto front(priority_tag<3>, R& r) noexcept(noexcept(r.front()))
    -> decltype(r.front())
{
    return r.front();
}

template<typename R>
constexpr auto front(priority_tag<2>, R& r) noexcept(noexcept(front(r)))
    -> decltype(front(r))
{
    return front(r);
}

template<typename R>
constexpr auto front(priority_tag<1>,
                     R& r) noexcept(noexcept(*matter::begin(r)))
    -> decltype(*matter::begin(r))
{
    return *matter::begin(r);
}
} // namespace front_impl

inline namespace cpo
{
constexpr auto front = [](auto&& range) noexcept(noexcept(
    ::matter::front_impl::front(max_priority_tag,
                                std::forward<decltype(range)>(range))))
                           -> decltype(::matter::front_impl::front(
                               max_priority_tag,
                               std::forward<decltype(range)>(range)))
{
    return ::matter::front_impl::front(max_priority_tag,
                                       std::forward<decltype(range)>(range));
};
} // namespace cpo

namespace back_impl
{
template<typename T, std::size_t N>
constexpr auto
    back(priority_tag<4>,
         T (&arr)[N]) noexcept(noexcept(*(arr + (std::extent_v<T[N]> - 1))))
        -> decltype(*(arr + (std::extent_v<T[N]> - 1)))
{
    return *(arr + (std::extent_v<T[N]> - 1));
}

template<typename R>
constexpr auto back(priority_tag<3>, R& r) noexcept(noexcept(r.back()))
    -> decltype(r.back())
{
    return r.back();
}

// customization point for back function
template<typename R>
constexpr auto back(priority_tag<2>, R& r) noexcept(noexcept(back(r)))
    -> decltype(back(r))
{
    return back(r);
}

// try going one back from the past the end iterator
template<typename R> // clang-format off
requires bidirectional_range<R> && common_range<R>
constexpr auto back(priority_tag<1>, // clang-format on
         R& r) noexcept(noexcept(*(--matter::end(r))))
        -> decltype(*(--matter::end(r)))
{
    return *(--matter::end(r));
}

// only activated in case end iterator is not decrementable (for example with
// sentinels)
template<typename R> // clang-format off
    requires random_access_range<R> && sized_range<R>
constexpr auto // clang-format on
        back(priority_tag<0>, R& r) noexcept(noexcept(*(matter::begin(r) +
                                                        (matter::size(r) - 1))))
            -> decltype(*(matter::begin(r) + (matter::size(r) - 1)))
{
    return *(matter::begin(r) + (matter::size(r) - 1));
}
} // namespace back_impl

inline namespace cpo
{
constexpr auto back = [](auto&& range) noexcept(noexcept(
    ::matter::back_impl::back(max_priority_tag,
                              std::forward<decltype(range)>(range))))
                          -> decltype(::matter::back_impl::back(
                              max_priority_tag,
                              std::forward<decltype(range)>(range)))
{
    return ::matter::back_impl::back(max_priority_tag,
                                     std::forward<decltype(range)>(range));
};
} // namespace cpo
} // namespace matter
