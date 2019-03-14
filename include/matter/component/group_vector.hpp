#ifndef MATTER_COMPONENT_GROUP_VECTOR_HPP
#define MATTER_COMPONENT_GROUP_VECTOR_HPP

#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "matter/component/group.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/util/id_erased.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
/// \brief stores all groups of a certain size
/// this is a specialised vector which stores groups, this datastructure allows
/// dynamic sized groups without needing templates to overly bloat the code.
class group_vector {
public:
    class iterator {
    public:
        friend class group_vector;

        using value_type        = group;
        using pointer           = group;
        using reference         = group;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = ssize_t;

        using vector_iterator_type =
            typename std::vector<matter::id_erased>::iterator;
        using vector_const_iterator_type =
            typename std::vector<matter::id_erased>::const_iterator;

    private:
        vector_iterator_type it_;
        std::size_t          size_;

    public:
        constexpr iterator(vector_iterator_type it, std::size_t size)
            : it_{it}, size_{size}
        {}

        bool operator==(const iterator& other) const noexcept
        {
            return it_ == other.it_ && size_ == other.size_;
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        iterator& operator++() noexcept
        {
            it_ += size_;
            return *this;
        }

        iterator& operator--() noexcept
        {
            it_ -= size_;
            return *this;
        }

        iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        iterator& operator+=(ssize_t movement) noexcept
        {
            it_ += (movement * size_);
            return *this;
        }

        iterator& operator-=(ssize_t movement) noexcept
        {
            it_ -= (movement * size_);
            return *this;
        }

        iterator operator+(ssize_t movement) const noexcept
        {
            auto r = *this;
            r += movement;
            return r;
        }

        iterator operator-(ssize_t movement) const noexcept
        {
            auto r = *this;
            r -= movement;
            return r;
        }

        difference_type operator-(const iterator& other) const noexcept
        {
            assert(size_ == other.size_);
            assert((it_ - other.it_) % size_ == 0);
            return (it_ - other.it_) / size_;
        }

        operator vector_iterator_type() const noexcept
        {
            return it_;
        }

        operator vector_const_iterator_type() const noexcept
        {
            return it_;
        }

        group operator*() noexcept
        {
            return group{std::addressof(*it_), size_};
        }
    };

public:
    using id_type = typename matter::id_erased::id_type;

private:
    /// the number of components each group stores
    const std::size_t              size_;
    std::vector<matter::id_erased> groups_{};

public:
    explicit group_vector(std::size_t size) : size_{size}
    {}

    iterator begin() noexcept
    {
        return {groups_.begin(), size_};
    }

    iterator end() noexcept
    {
        return {groups_.end(), size_};
    }

    std::size_t group_size() const noexcept
    {
        return size_;
    }

    template<typename... Ts>
    group emplace(const unordered_typed_ids<id_type, Ts...>& ids) noexcept(
        (std::is_nothrow_default_constructible_v<
             matter::component_storage_t<typename Ts::type>> &&
         ...))
    {
        static_assert((std::is_default_constructible_v<
                           matter::component_storage_t<typename Ts::type>> &&
                       ...),
                      "Component storage for one of the Cs... is not default "
                      "constructible");
        assert(size_ == ids.size());

        auto ordered_ids = ordered_typed_ids{ids};

        auto insertion_point = std::lower_bound(begin(), end(), ordered_ids);
        assert([&]() {
            if (insertion_point != end())
            {
                return !(*insertion_point).contains(ordered_ids);
            }

            return true;
        }());

        std::array<matter::id_erased, sizeof...(Ts)> groups{matter::id_erased{
            ids.template get<Ts>(),
            std::in_place_type_t<
                matter::component_storage_t<typename Ts::type>>{}}...};

        auto inserted_at =
            groups_.insert(insertion_point,
                           std::make_move_iterator(groups.begin()),
                           std::make_move_iterator(groups.end()));

        // sort them into order for future comparison
        std::sort(inserted_at, inserted_at + size_);
        return *iterator(inserted_at, size_);
    }

    group operator[](std::size_t index) noexcept
    {
        assert(index <= size());
        return group{std::addressof(groups_[size_ * index]), size_};
    }

    std::size_t size() const noexcept
    {
        assert((groups_.size() % size_) == 0);
        return groups_.size() / size_;
    }

    bool empty() const noexcept
    {
        return groups_.empty();
    }
};
} // namespace matter

#endif
