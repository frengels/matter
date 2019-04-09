#ifndef MATTER_UTIL_ITERATOR_HPP
#define MATTER_UTIL_ITERATOR_HPP

#pragma once

#include <iterator>
#include <utility>

#include "matter/util/concepts.hpp"

namespace matter
{
template<typename T>
using iterator_t = decltype(std::declval<T>().begin());

template<typename T>
using const_iterator_t = decltype(std::declval<T>().cbegin());

template<typename T>
using reverse_iterator_t = decltype(std::declval<T>().rbegin());

template<typename T>
using const_reverse_iterator_t = decltype(std::declval<T>().crbegin());

template<typename T>
using sentinel_t = decltype(std::declval<T>().end());

template<typename T>
using const_sentinel_t = decltype(std::declval<T>().cend());

template<typename T>
using reverse_sentinel_t = decltype(std::declval<T>().rend());

template<typename T>
using const_reverse_sentinel_t = decltype(std::declval<T>().crend());

template<typename Iter>
using iter_reference_t = decltype(*std::declval<Iter&>());

template<typename Iterator, typename Sentinel, typename = void>
struct is_simd_iterator : std::false_type
{};

template<typename Iterator, typename Sentinel>
struct is_simd_iterator<
    Iterator,
    Sentinel,
    std::enable_if_t<has_less_than<Iterator, Sentinel>::value &&
                         has_add<Iterator,
                                 typename std::iterator_traits<
                                     Iterator>::difference_type>::value &&
                         has_minus<Iterator, Sentinel>::value &&
                         has_pre_increment<Iterator>::value,
                     std::void_t<iter_reference_t<Iterator>>>> : std::true_type
{};
} // namespace matter

#endif
