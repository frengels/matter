#ifndef MATTER_UTIL_ALGORITHM_HPP
#define MATTER_UTIL_ALGORITHM_HPP

#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <utility>

namespace matter
{
namespace detail
{
// used to have constexpr property until c++20
template<typename T>
constexpr void swap_if_less(T& lhs, T& rhs) noexcept
{
    T t = lhs < rhs ? std::move(lhs) : std::move(rhs);
    rhs = lhs < rhs ? std::move(rhs) : std::move(lhs);
    lhs = std::move(t);
}
} // namespace detail

// simply a is_sorted that is constexpr until c++20 has it
template<typename ForwardIt>
constexpr bool is_sorted(ForwardIt first, ForwardIt last) noexcept
{
    if (first != last)
    {
        ForwardIt next = first;
        while (++next != last)
        {
            if (*next < *first)
            {
                return false;
            }
            first = next;
        }
    }

    return true;
}

template<typename InputIt1, typename InputIt2>
constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) noexcept
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            return false;
        }
    }

    return true;
}

template<typename T>
constexpr void swap(T& lhs, T& rhs) noexcept
{
    T tmp = std::move(lhs);
    lhs   = std::move(rhs);
    rhs   = std::move(tmp);
}

template<typename ForwardIt1, typename ForwardIt2>
constexpr void iter_swap(ForwardIt1 a, ForwardIt2 b) noexcept
{
    using ::matter::swap;
    swap(*a, *b);
}

template<typename ForwardIt>
constexpr ForwardIt
rotate(ForwardIt first, ForwardIt n_first, ForwardIt last) noexcept
{
    if (first == n_first)
    {
        return last;
    }
    if (n_first == last)
    {
        return first;
    }
    ForwardIt read      = n_first;
    ForwardIt write     = first;
    ForwardIt next_read = first;

    while (read != last)
    {
        if (write == next_read)
        {
            next_read = read;
        }
        matter::iter_swap(write++, read++);
    }

    rotate(write, next_read, last);
    return write;
}

template<typename ForwardIt, typename T, typename Compare = std::less<T>>
constexpr ForwardIt upper_bound(ForwardIt first,
                                ForwardIt last,
                                const T&  value,
                                Compare   comp = std::less<T>{}) noexcept
{
    ForwardIt                                                 it = first;
    typename std::iterator_traits<ForwardIt>::difference_type count =
                                                                  std::distance(
                                                                      first,
                                                                      last),
                                                              step = count / 2;
    count = std::distance(first, last);

    while (count > 0)
    {
        it   = first;
        step = count / 2;
        std::advance(it, step);
        if (!comp(value, *it))
        {
            first = ++it;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return first;
}

template<typename ForwardIt>
constexpr void insertion_sort(ForwardIt begin, ForwardIt last) noexcept
{
    for (auto it = begin; it != last; ++it)
    {
        matter::rotate(matter::upper_bound(begin, it, *it), it, it + 1);
    }
}

template<typename T, std::size_t N>
constexpr void static_sort(std::array<T, N>& arr) noexcept
{
    // too lazy to figure out how these are automatically generated
    using ::matter::detail::swap_if_less;
    if constexpr (N == 1)
    {
        return;
    }
    else if constexpr (N == 2)
    {
        swap_if_less(arr[0], arr[1]);
    }
    else if constexpr (N == 3)
    {
        swap_if_less(arr[1], arr[2]);
        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[0], arr[1]);
    }
    else if constexpr (N == 4)
    {
        swap_if_less(arr[0], arr[1]);
        swap_if_less(arr[2], arr[3]);

        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[1], arr[3]);

        swap_if_less(arr[1], arr[2]);
    }
    else if constexpr (N == 5)
    {
        swap_if_less(arr[0], arr[1]);
        swap_if_less(arr[3], arr[4]);

        swap_if_less(arr[2], arr[4]);

        swap_if_less(arr[2], arr[3]);
        swap_if_less(arr[1], arr[4]);

        swap_if_less(arr[0], arr[3]);

        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[1], arr[3]);

        swap_if_less(arr[1], arr[2]);
    }
    else if constexpr (N == 6)
    {
        swap_if_less(arr[1], arr[2]);
        swap_if_less(arr[4], arr[5]);

        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[3], arr[5]);

        swap_if_less(arr[0], arr[1]);
        swap_if_less(arr[3], arr[4]);
        swap_if_less(arr[2], arr[5]);

        swap_if_less(arr[0], arr[3]);
        swap_if_less(arr[1], arr[4]);

        swap_if_less(arr[2], arr[4]);
        swap_if_less(arr[1], arr[3]);

        swap_if_less(arr[2], arr[3]);
    }
    else if constexpr (N == 7)
    {
        swap_if_less(arr[1], arr[2]);
        swap_if_less(arr[3], arr[4]);
        swap_if_less(arr[5], arr[6]);

        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[3], arr[5]);
        swap_if_less(arr[4], arr[6]);

        swap_if_less(arr[0], arr[1]);
        swap_if_less(arr[4], arr[5]);
        swap_if_less(arr[2], arr[6]);

        swap_if_less(arr[0], arr[4]);
        swap_if_less(arr[1], arr[5]);

        swap_if_less(arr[0], arr[3]);
        swap_if_less(arr[2], arr[5]);

        swap_if_less(arr[1], arr[3]);
        swap_if_less(arr[2], arr[4]);

        swap_if_less(arr[2], arr[3]);
    }
    else if constexpr (N == 8)
    {
        swap_if_less(arr[0], arr[1]);
        swap_if_less(arr[2], arr[3]);
        swap_if_less(arr[4], arr[5]);
        swap_if_less(arr[6], arr[7]);

        swap_if_less(arr[0], arr[2]);
        swap_if_less(arr[1], arr[3]);
        swap_if_less(arr[4], arr[6]);
        swap_if_less(arr[5], arr[7]);

        swap_if_less(arr[1], arr[2]);
        swap_if_less(arr[5], arr[6]);
        swap_if_less(arr[0], arr[4]);
        swap_if_less(arr[3], arr[7]);

        swap_if_less(arr[1], arr[5]);
        swap_if_less(arr[2], arr[6]);

        swap_if_less(arr[1], arr[4]);
        swap_if_less(arr[3], arr[6]);

        swap_if_less(arr[2], arr[4]);
        swap_if_less(arr[3], arr[5]);

        swap_if_less(arr[3], arr[4]);
    }
    else
    {
        insertion_sort(arr.begin(), arr.end());
    }
}
} // namespace matter

#endif