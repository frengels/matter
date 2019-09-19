#pragma once

#include "matter/concepts/common_reference_with.hpp"
#include "matter/concepts/default_constructible.hpp"
#include "matter/concepts/regular.hpp"
#include "matter/concepts/same_as.hpp"
#include "matter/concepts/semiregular.hpp"
#include "matter/concepts/signed_integral.hpp"
#include "matter/concepts/totally_ordered.hpp"
#include "matter/concepts/weakly_equality_comparable_with.hpp"
#include "matter/iterator/common.hpp"
#include "matter/iterator/traits.hpp"

namespace matter
{
namespace detail
{
template<typename T>
constexpr bool is_signed_integer_like = signed_integral<T>;
// TODO http://eel.is/c++draft/iterators#iterator.concept.winc-2 to model
// integer-class type.
} // namespace detail

template<typename I>
concept weakly_incrementable = // clang-format off
    matter::default_constructible<I>&&
    matter::movable<I>&& 
    requires(I i)
    {
        typename matter::iter_difference_t<I>;
        requires detail::is_signed_integer_like<matter::iter_difference_t<I>>;
        { ++i };
        requires same_as<decltype(++i), I&>;
        i++;
    }; // clang-format on

template<typename I>
concept incrementable = // clang-format off
    regular<I> &&
    weakly_incrementable<I> &&
    requires(I i)
    {
        { i++ } -> same_as<I>;
    }; // clang-format on

template<typename I>
concept input_or_output_iterator = // clang-format off
    requires(I i)
    {
        { *i } -> can_reference;
    } && weakly_incrementable<I>; // clang-format on

template<typename In>
concept readable = // clang-format off
    requires
    {
        typename iter_value_t<In>;
        typename iter_reference_t<In>;
        typename iter_rvalue_reference_t<In>;
    } &&
    common_reference_with<iter_reference_t<In>&&, iter_value_t<In>&> &&
    common_reference_with<iter_reference_t<In>&&, iter_rvalue_reference_t<In>&&> &&
    common_reference_with<iter_rvalue_reference_t<In>&&, const iter_value_t<In>&>; // clang-format on

template<typename Out, typename T>
concept writable = // clang-format off
    requires(Out&& o, T&& t)
    {
        *o = std::forward<T>(t);
        *std::forward<Out>(o) = std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*o) =
            std::forward<T>(t);
        const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(o)) =
            std::forward<T>(t);
    }; // clang-format on

template<typename I>
concept input_iterator = // clang-format off
    input_or_output_iterator<I> &&
    readable<I>; // clang-format on
// consciously leaving out the iterator_traits stuff

template<typename I, typename T>
concept output_iterator = // clang-format off
    input_or_output_iterator<I> &&
    writable<I, T> &&
    requires(I i, T&& t)
    {
        *i++ = std::forward<T>(t);
    }; // clang-format on

template<typename S, typename I>
concept sentinel_for = semiregular<S>&& input_or_output_iterator<I>&&
                                        weakly_equality_comparable_with<S, I>;

template<typename S, typename I>
inline constexpr bool disable_sized_sentinel_for = false;

template<typename S, typename I>
concept sized_sentinel_for =
    sentinel_for<S, I> &&
    !disable_sized_sentinel_for<std::remove_cv_t<S>,
                                std::remove_cv_t<I>> && // clang-format off
    requires(const I& i, const S& s)
    {
        { s - i } -> same_as<iter_difference_t<I>>;
        { i - s } -> same_as<iter_difference_t<I>>;
    }; // clang-format on

template<typename I>
concept forward_iterator = // clang-format off
    input_iterator<I> &&
    incrementable<I> &&
    sentinel_for<I, I>; // clang-format on

template<typename I>
concept bidirectional_iterator = // clang-format off
    forward_iterator<I> &&
    requires(I i)
    {
        { --i };
        requires same_as<decltype(--i), I&>;
        { i-- };
        requires same_as<decltype(i--), I>;
    }; // clang-format on

template<typename I>
concept random_access_iterator = // clang-format off
    bidirectional_iterator<I> &&
    totally_ordered<I> &&
    sized_sentinel_for<I, I> &&
    requires(I i, const I j, const iter_difference_t<I> n)
    {
        { i += n };
        requires same_as<decltype(i += n), I&>;
        { j +  n };
        requires same_as<decltype(j + n), I>;
        { n +  j };
        requires same_as<decltype(n + j), I>;
        { i -= n };
        requires same_as<decltype(i -= n), I&>;
        { j -  n };
        requires same_as<decltype(j - n), I>;
        { j[n] };
        requires same_as<decltype(j[n]), iter_reference_t<I>>;
    };
} // namespace matter