#ifndef MATTER_COMPONENT_UNTYPED_ID_HPP
#define MATTER_COMPONENT_UNTYPED_ID_HPP

#pragma once

#include <algorithm>

#include "matter/util/algorithm.hpp"

namespace matter
{
template<typename Id>
struct unordered_untyped_ids
{
    using id_type        = Id;
    using size_type      = std::size_t;
    using iterator       = id_type*;
    using const_iterator = const id_type*;

private:
    id_type*  ids_;
    size_type size_;

public:
    constexpr unordered_untyped_ids(id_type* ids, size_type size) noexcept
        : ids_{ids}, size_{size}
    {}

    constexpr iterator begin() noexcept
    {
        return data();
    }

    constexpr iterator end() noexcept
    {
        return data() + size();
    }

    constexpr const_iterator begin() const noexcept
    {
        return data();
    }

    constexpr const_iterator end() const noexcept
    {
        return data() + size();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return data();
    }

    constexpr const_iterator cend() const noexcept
    {
        return data() + size();
    }

    constexpr id_type* data() noexcept
    {
        return ids_;
    }

    constexpr const id_type data() const noexcept
    {
        return ids_;
    }

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    constexpr id_type& operator[](size_type index) noexcept
    {
        assert(index < size());
        return ids_[index];
    }

    constexpr const id_type& operator[](size_type index) const noexcept
    {
        assert(index < size());
        return ids_[index];
    }
};

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
        matter::insertion_sort(ids_, ids_ + this->size());
    }

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
