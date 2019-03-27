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
template<typename UnorderedTypedIds>
struct group_vector_view;

/// \brief stores all groups of a certain size
/// this is a specialised vector which stores groups, this datastructure allows
/// dynamic sized groups without needing templates to overly bloat the code.
class group_vector {
private:
    template<typename UnorderedTypedIds>
    friend struct group_vector_view;

    template<bool Const>
    class iterator_ {
    public:
        friend class group_vector;

        using vector_iterator_type = std::conditional_t<
            Const,
            typename std::vector<matter::id_erased>::const_iterator,
            typename std::vector<matter::id_erased>::iterator>;

        using value_type =
            typename std::iterator_traits<vector_iterator_type>::value_type;
        using pointer =
            typename std::iterator_traits<vector_iterator_type>::pointer;
        using reference =
            typename std::iterator_traits<vector_iterator_type>::reference;
        using iterator_category = typename std::iterator_traits<
            vector_iterator_type>::iterator_category;
        using difference_type = typename std::iterator_traits<
            vector_iterator_type>::difference_type;

    private:
        vector_iterator_type it_;
        std::size_t          size_;

    private:
        constexpr iterator_(vector_iterator_type it, std::size_t size)
            : it_{it}, size_{size}
        {}

    public:
        auto group_size() const noexcept
        {
            return size_;
        }

        constexpr operator vector_iterator_type() const noexcept
        {
            return it_;
        }

        bool operator==(const iterator_& other) const noexcept
        {
            // it's impossible to iterate with a different size on the same
            // iterator so disregard checking for equality there
            return it_ == other.it_;
        }

        bool operator!=(const iterator_& other) const noexcept
        {
            return !(*this == other);
        }

        auto operator==(const vector_iterator_type& it) const noexcept
        {
            return it_ == it;
        }

        auto operator!=(const vector_iterator_type& it) const noexcept
        {
            return !(*this == it);
        }

        iterator_& operator++() noexcept
        {
            it_ += size_;
            return *this;
        }

        iterator_& operator--() noexcept
        {
            it_ -= size_;
            return *this;
        }

        iterator_ operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator_ operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        iterator_& operator+=(ssize_t movement) noexcept
        {
            it_ += (movement * size_);
            return *this;
        }

        iterator_& operator-=(ssize_t movement) noexcept
        {
            it_ -= (movement * size_);
            return *this;
        }

        iterator_ operator+(ssize_t movement) const noexcept
        {
            auto r = *this;
            r += movement;
            return r;
        }

        iterator_ operator-(ssize_t movement) const noexcept
        {
            auto r = *this;
            r -= movement;
            return r;
        }

        difference_type operator-(const iterator_& other) const noexcept
        {
            assert(size_ == other.size_);
            assert((it_ - other.it_) % size_ == 0);
            return (it_ - other.it_) / size_;
        }

        matter::id_erased* operator->() noexcept
        {
            return it_.operator->();
        }

        const matter::id_erased* operator->() const noexcept
        {
            return it_.operator->();
        }

        matter::id_erased& operator*() noexcept
        {
            return *it_;
        }

        const matter::id_erased& operator*() const noexcept
        {
            return *it_;
        }
    };

public:
    using id_type = typename matter::id_erased::id_type;

    using const_iterator = iterator_<true>;
    using iterator       = iterator_<false>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;

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

    const_iterator begin() const noexcept
    {
        return {groups_.begin(), size_};
    }

    const_iterator end() const noexcept
    {
        return {groups_.end(), size_};
    }

    const_iterator cbegin() const noexcept
    {
        return {groups_.cbegin(), size_};
    }

    const_iterator cend() const noexcept
    {
        return {groups_.cend(), size_};
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{end()};
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator{begin()};
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{cend()};
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{cbegin()};
    }

    std::size_t group_size() const noexcept
    {
        return size_;
    }

    template<typename... Ts>
    exact_group<id_type, typename Ts::type...>
    emplace(const unordered_typed_ids<id_type, Ts...>& ids) noexcept(
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

        auto insertion_point = lower_bound(ordered_ids);

        auto inserted_at = emplace_at(insertion_point, ids);
        auto grp         = any_group{std::addressof(*inserted_at), size_};
        return exact_group{ids, grp};
    }

    template<typename... TIds>
    const_iterator
    find(const matter::ordered_typed_ids<id_type, TIds...>& ids) const noexcept
    {
        assert(ids.size() == group_size());

        auto it = lower_bound(ids);

        if (it == end())
        {
            return it;
        }

        auto grp = const_any_group{*it, group_size()};
        return grp == ids ? it : end();
    }

    /// \brief finds an exact match for the ids, otherwise end()
    /// the passed ids must be the exact same size as the groups managed by this
    /// group_vector. To retrieve groups which contain the requested components
    /// you should use a group_vector_view<T>.
    template<typename... TIds>
    iterator
    find(const matter::ordered_typed_ids<id_type, TIds...>& ids) noexcept
    {
        assert(ids.size() == group_size());

        auto it = lower_bound(ids);

        if (it == end())
        {
            return it;
        }

        auto grp = const_any_group{*it, group_size()};
        return grp == ids ? it : end();
    }

    template<typename... TIds>
    std::optional<exact_group<id_type, typename TIds::type...>> find_group(
        const matter::unordered_typed_ids<id_type, TIds...>& ids,
        const matter::ordered_typed_ids<id_type, TIds...>& ordered_ids) noexcept
    {
        auto it = find(ordered_ids);

        if (it == end())
        {
            return {};
        }

        auto grp = any_group{*it, group_size()};
        return exact_group{ids, grp};
    }

    template<typename... TIds>
    std::optional<exact_group<id_type, typename TIds::type...>> find_group(
        const matter::unordered_typed_ids<id_type, TIds...>& ids) noexcept
    {
        return find_group(ids, ordered_typed_ids{ids});
    }

    /// \brief same as find, but emplaces the group if not found
    template<typename... TIds>
    iterator find_emplace(
        const matter::unordered_typed_ids<id_type, TIds...>& unordered_ids,
        const matter::ordered_typed_ids<id_type, TIds...>& ordered_ids) noexcept
    {
        assert(ordered_ids.size() == group_size());

        auto it = lower_bound(ordered_ids);

        if (it == end())
        {
            return emplace_at(it, unordered_ids);
        }
        else if (auto grp = const_any_group{*it, group_size()};
                 !grp.contains(ordered_ids))
        {
            return emplace_at(it, unordered_ids);
        }
        else
        {
            return it;
        }
    }

    template<typename... TIds>
    iterator find_emplace(const matter::unordered_typed_ids<id_type, TIds...>&
                              unordered_ids) noexcept
    {
        return find_emplace(unordered_ids,
                            matter::ordered_typed_ids{unordered_ids});
    }

    template<typename... TIds>
    any_group find_emplace_group(
        const matter::unordered_typed_ids<id_type, TIds...>& unordered_ids,
        const matter::ordered_typed_ids<id_type, TIds...>& ordered_ids) noexcept
    {
        auto it = find_emplace(unordered_ids, ordered_ids);
        return any_group{*it, group_size()};
    }

    template<typename... TIds>
    any_group
    find_emplace_group(const matter::unordered_typed_ids<id_type, TIds...>&
                           unordered_ids) noexcept
    {
        return find_emplace_group(unordered_ids,
                                  matter::ordered_typed_ids{unordered_ids});
    }

    any_group operator[](std::size_t index) noexcept
    {
        assert(index <= size());
        return any_group{std::addressof(groups_[size_ * index]), size_};
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

private:
    template<typename... TIds>
    iterator emplace_at(
        iterator pos,
        const unordered_typed_ids<id_type, TIds...>&
            unordered_ids) noexcept((std::
                                         is_nothrow_default_constructible_v<
                                             matter::component_storage_t<
                                                 typename TIds::type>> &&
                                     ...))
    {
        assert(([&]() {
            if (pos != end())
            {
                auto grp = const_any_group{*pos, size_};
                return !grp.contains(matter::ordered_typed_ids{unordered_ids});
            }
            return true;
        }()));

        std::array<matter::id_erased, sizeof...(TIds)> stores{matter::id_erased{
            unordered_ids.template get<TIds>(),
            std::in_place_type_t<
                matter::component_storage_t<typename TIds::type>>{}}...};

        auto inserted_at =
            groups_.insert(pos.it_,
                           std::make_move_iterator(stores.begin()),
                           std::make_move_iterator(stores.end()));

        std::sort(inserted_at, inserted_at + size_);
        return {inserted_at, group_size()};
    }

    template<typename... TIds>
    const_iterator
    lower_bound(const matter::ordered_typed_ids<id_type, TIds...>& ids) const
        noexcept
    {
        return std::lower_bound(
            begin(),
            end(),
            ids,
            [size = group_size()](const auto& erased, const auto& ids) {
                auto grp = const_any_group{erased, size};
                return grp < ids;
            });
    }

    template<typename... TIds>
    iterator
    lower_bound(const matter::ordered_typed_ids<id_type, TIds...>& ids) noexcept
    {
        return std::lower_bound(
            begin(),
            end(),
            ids,
            [size = group_size()](const auto& erased, const auto& ids) {
                auto grp = const_any_group{erased, size};
                return grp < ids;
            });
    }
};
} // namespace matter

#endif
