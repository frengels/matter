#ifndef MATTER_COMPONENT_UNTYPED_ID_HPP
#define MATTER_COMPONENT_UNTYPED_ID_HPP

#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include "matter/id/typed_id.hpp"
#include "matter/util/algorithm.hpp"

namespace matter
{
template<typename Id>
struct ordered_untyped_ids
{
    using id_type   = Id;
    using size_type = std::size_t;

    using iterator       = id_type*;
    using const_iterator = const id_type*;

private:
    const id_type* ids_;
    size_type      size_;

public:
    constexpr ordered_untyped_ids(const id_type* ids, size_type size) noexcept
        : ids_{ids}, size_{size}
    {
        // passed ids must be sorted
        assert(std::is_sorted(ids_, ids_ + size));
        // passed ids must all be unique
        assert(std::adjacent_find(ids, ids_ + size) == (ids_ + size));
    }

    explicit constexpr ordered_untyped_ids(
        const std::vector<id_type>& ids) noexcept
        : ordered_untyped_ids{ids.data(), ids.size()}
    {
        // the main constructor asserts for us
    }

    template<std::size_t N>
    explicit constexpr ordered_untyped_ids(
        const std::array<id_type, N>& ids) noexcept
        : ordered_untyped_ids{ids.data(), ids.size()}
    {}

    template<typename... Ts>
    explicit constexpr ordered_untyped_ids(
        const matter::ordered_typed_ids<id_type, Ts...>& ids) noexcept
        : ordered_untyped_ids{ids.data(), ids.size()}
    {}

    constexpr const_iterator begin() const noexcept
    {
        return ids_;
    }

    constexpr const_iterator end() const noexcept
    {
        return ids_ + size_;
    }

    constexpr bool operator==(const ordered_untyped_ids<id_type>& other) const
        noexcept
    {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    constexpr bool operator!=(const ordered_untyped_ids<id_type>& other) const
        noexcept
    {
        return !(*this == other);
    }

    template<typename... Ts>
    constexpr bool
    operator==(const matter::ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        return std::equal(begin(), end(), ids.begin(), ids.end());
    }

    template<typename... Ts>
    constexpr bool
    operator!=(const matter::ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        return !(*this == ids);
    }

    constexpr bool operator<(const ordered_untyped_ids<id_type>& other) const
        noexcept
    {
        if (size() < other.size())
        {
            return true;
        }
        else if (size() > other.size())
        {
            return false;
        }
        else
        {
            for (std::size_t i = 0; i < size(); ++i)
            {
                if (operator[](i) < other[i])
                {
                    return true;
                }
            }
        }

        return false;
    }

    constexpr bool operator>(const ordered_untyped_ids<id_type>& other) const
        noexcept
    {
        if (size() > other.size())
        {
            return true;
        }
        else if (size() < other.size())
        {
            return false;
        }
        else
        {
            for (std::size_t i = 0; i < size(); ++i)
            {
                if (operator[](i) > other[i])
                {
                    return true;
                }
            }
        }

        return false;
    }

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    constexpr const id_type* data() const noexcept
    {
        return ids_;
    }

    constexpr const id_type& operator[](std::size_t idx) const noexcept
    {
        assert(idx < size());
        return ids_[idx];
    }

    constexpr bool contains(matter::ordered_untyped_ids<Id> other) const
        noexcept
    {}
};
} // namespace matter

#endif
