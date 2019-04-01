#ifndef MATTER_UTIL_ITERATOR_HPP
#define MATTER_UTIL_ITERATOR_HPP

#pragma once

#include <utility>

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
using iter_reference_t = decltype(*std::declval<Iter>());
} // namespace matter

#endif
