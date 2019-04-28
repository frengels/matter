#ifndef MATTER_COMPONENT_UNTYPED_ID_HPP
#define MATTER_COMPONENT_UNTYPED_ID_HPP

#pragma once

#include <algorithm>
#include <array>
#include <vector>

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
    id_type*  ids_;
    size_type size_;

public:
    constexpr ordered_untyped_ids(id_type* ids, size_type size) noexcept
        : ids_{ids}, size_{size}
    {
        assert(std::is_sorted(ids_, ids_ + size));
    }

    explicit constexpr ordered_untyped_ids(std::vector<id_type>& ids) noexcept
        : ordered_untyped_ids{ids.data(), ids.size()}
    {}

    template<std::size_t N>
    explicit constexpr ordered_untyped_ids(std::array<id_type, N>& ids) noexcept
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

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    constexpr const id_type* data() const noexcept
    {
        return ids_;
    }

    constexpr id_type& operator[](std::size_t idx) noexcept
    {
        assert(idx < size());
        return ids_[idx];
    }

    constexpr const id_type& operator[](std::size_t idx) const noexcept
    {
        assert(idx < size());
        return ids_[idx];
    }
};
} // namespace matter

#endif
