#ifndef MATTER_COMPONENT_GROUP_VECTOR_HPP
#define MATTER_COMPONENT_GROUP_VECTOR_HPP

#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "matter/component/group.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/storage/erased_storage.hpp"
#include "matter/util/iterator.hpp"
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

public:
    struct iterator
    {
        using vector_iterator_type =
            matter::iterator_t<std::vector<matter::erased_storage>>;
        using group_size_iterator_type =
            matter::iterator_t<std::vector<std::size_t>>;

        using value_type        = any_group;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = typename std::iterator_traits<
            vector_iterator_type>::iterator_category;
        using difference_type = typename std::iterator_traits<
            vector_iterator_type>::difference_type;

    private:
        vector_iterator_type     it_;
        group_size_iterator_type size_it_;

        std::size_t group_size_;

    public:
        iterator() noexcept = default;

        constexpr iterator(vector_iterator_type     it,
                           group_size_iterator_type size_it,
                           std::size_t              group_size) noexcept
            : it_{it}, size_it_{size_it}, group_size_{group_size}
        {}

        auto base() const noexcept
        {
            return it_;
        }

        auto group_size() const noexcept
        {
            return group_size_;
        }

        bool operator==(const iterator& other) const noexcept
        {
            // it's impossible to iterate with a different size on the same
            // iterator so disregard checking for equality there
            return it_ == other.it_;
        }

        bool operator!=(const iterator& other) const noexcept
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

        iterator& operator++() noexcept
        {
            it_ += group_size();
            ++size_it_;
            return *this;
        }

        iterator& operator--() noexcept
        {
            it_ -= group_size();
            --size_it_;
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

        iterator& operator+=(difference_type movement) noexcept
        {
            it_ += (movement * group_size());
            size_it_ += movement;
            return *this;
        }

        iterator& operator-=(difference_type movement) noexcept
        {
            it_ -= (movement * group_size());
            size_it_ -= movement;
            return *this;
        }

        iterator operator+(difference_type movement) const noexcept
        {
            auto r = *this;
            r += movement;
            return r;
        }

        iterator operator-(difference_type movement) const noexcept
        {
            auto r = *this;
            r -= movement;
            return r;
        }

        difference_type operator-(const iterator& other) const noexcept
        {
            assert(group_size() == other.group_size());
            assert((it_ - other.it_) % group_size() == 0);
            return size_it_ - other.size_it_;
        }

        reference operator*() const noexcept
        {
            return {*it_, *size_it_, group_size()};
        }
    };

    struct const_iterator
    {
        using vector_const_iterator_type =
            matter::const_iterator_t<std::vector<matter::erased_storage>>;
        using group_size_const_iterator_type =
            matter::const_iterator_t<std::vector<std::size_t>>;

        using value_type        = const_any_group;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = typename std::iterator_traits<
            vector_const_iterator_type>::iterator_category;
        using difference_type = typename std::iterator_traits<
            vector_const_iterator_type>::difference_type;

    private:
        vector_const_iterator_type     it_;
        group_size_const_iterator_type size_it_;
        std::size_t                    size_;

    public:
        const_iterator() noexcept = default;

        constexpr const_iterator(vector_const_iterator_type     it,
                                 group_size_const_iterator_type size_it,
                                 std::size_t                    size) noexcept
            : it_{it}, size_it_{size_it}, size_{size}
        {}

        const_iterator(const iterator& it) noexcept
            : it_{it.base()}, size_{it.group_size()}
        {}

        const_iterator& operator=(const iterator& it) noexcept
        {
            it_   = it.base();
            size_ = it.group_size();

            return *this;
        }

        auto base() const noexcept
        {
            return it_;
        }

        auto group_size() const noexcept
        {
            return size_;
        }

        auto operator==(const const_iterator& other) const noexcept
        {
            return base() == other.base();
        }

        friend auto operator==(const iterator&       lhs,
                               const const_iterator& rhs) noexcept
        {
            return lhs.base() == rhs.base();
        }

        auto operator!=(const const_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        friend auto operator!=(const iterator&       lhs,
                               const const_iterator& rhs) noexcept
        {
            return lhs.base() != rhs.base();
        }

        const_iterator& operator++() noexcept
        {
            it_ += group_size();
            ++size_it_;
            return *this;
        }

        const_iterator& operator--() noexcept
        {
            it_ -= group_size();
            --size_it_;
            return *this;
        }

        const_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator& operator+=(difference_type movement) noexcept
        {
            it_ += (movement * group_size());
            size_it_ += movement;
            return *this;
        }

        const_iterator& operator-=(difference_type movement) noexcept
        {
            it_ -= (movement * group_size());
            size_it_ -= movement;
            return *this;
        }

        const_iterator operator+(difference_type movement) const noexcept
        {
            auto r = *this;
            r += movement;
            return r;
        }

        const_iterator operator-(difference_type movement) const noexcept
        {
            auto r = *this;
            r -= movement;
            return r;
        }

        difference_type operator-(const const_iterator& other) const noexcept
        {
            assert(group_size() == other.group_size());
            assert((base() - other.base()) % group_size() == 0);
            return (size_it_ - other.size_it_);
        }

        reference operator*() const noexcept
        {
            return {*it_, *size_it_, size_};
        }
    };

    struct reverse_iterator
    {
        using vector_reverse_iterator_type =
            matter::reverse_iterator_t<std::vector<matter::erased_storage>>;
        using group_size_reverse_iterator_type =
            matter::reverse_iterator_t<std::vector<std::size_t>>;

        using value_type        = any_group;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = typename std::iterator_traits<
            vector_reverse_iterator_type>::iterator_category;
        using difference_type = typename std::iterator_traits<
            vector_reverse_iterator_type>::difference_type;

    private:
        vector_reverse_iterator_type     it_;
        group_size_reverse_iterator_type size_it_;
        std::size_t                      size_;

    public:
        reverse_iterator() = default;

        constexpr reverse_iterator(vector_reverse_iterator_type     it,
                                   group_size_reverse_iterator_type size_it,
                                   std::size_t size) noexcept
            : it_{it}, size_it_{size_it}, size_{size}
        {}

        auto base() const noexcept
        {
            return it_;
        }

        auto group_size() const noexcept
        {
            return size_;
        }

        constexpr auto operator==(const reverse_iterator& other) const noexcept
        {
            return it_ == other.it_;
        }

        constexpr auto operator!=(const reverse_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr auto operator==(const vector_reverse_iterator_type& it) const
            noexcept
        {
            return it_ == it;
        }

        constexpr auto operator!=(const vector_reverse_iterator_type& it) const
            noexcept
        {
            return !(*this == it);
        }

        reverse_iterator& operator++() noexcept
        {
            it_ += size_;
            return *this;
        }

        reverse_iterator& operator--() noexcept
        {
            it_ -= size_;
            return *this;
        }

        reverse_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        reverse_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        reverse_iterator& operator+=(difference_type movement) noexcept
        {
            it_ += (movement * size_);
            return *this;
        }

        reverse_iterator& operator-=(difference_type movement) noexcept
        {
            it_ -= (movement * size_);
            return *this;
        }

        difference_type operator-(const reverse_iterator& other) const noexcept
        {
            assert(size_ == other.size_);
            assert((it_ - other.it_) % size_ == 0);

            return (it_ - other.it_) / size_;
        }

        reference operator*() const noexcept
        {
            return {*it_, *size_it_, size_};
        }
    };

    struct const_reverse_iterator
    {
        using vector_const_reverse_iterator_type =
            matter::const_reverse_iterator_t<
                std::vector<matter::erased_storage>>;
        using group_size_const_reverse_iterator_type =
            matter::const_reverse_iterator_t<std::vector<std::size_t>>;

        using value_type        = const_any_group;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = typename std::iterator_traits<
            vector_const_reverse_iterator_type>::iterator_category;
        using difference_type = typename std::iterator_traits<
            vector_const_reverse_iterator_type>::difference_type;

    private:
        vector_const_reverse_iterator_type     it_;
        group_size_const_reverse_iterator_type size_it_;
        std::size_t                            size_;

    public:
        const_reverse_iterator() = default;

        constexpr const_reverse_iterator(vector_const_reverse_iterator_type it,
                                         std::size_t size) noexcept
            : it_{it}, size_{size}
        {}

        const_reverse_iterator(const reverse_iterator& rit) noexcept
            : it_{rit.base()}, size_{rit.group_size()}
        {}

        const_reverse_iterator& operator=(const reverse_iterator& rit) noexcept
        {
            it_   = rit.base();
            size_ = rit.group_size();

            return *this;
        }

        auto base() const noexcept
        {
            return it_;
        }

        auto group_size() const noexcept
        {
            return size_;
        }

        auto operator==(const const_reverse_iterator& other) const noexcept
        {
            return base() == other.base();
        }

        friend auto operator==(const reverse_iterator&       lhs,
                               const const_reverse_iterator& rhs) noexcept
        {
            return lhs.base() == rhs.base();
        }

        auto operator!=(const const_reverse_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        friend auto operator!=(const reverse_iterator&       lhs,
                               const const_reverse_iterator& rhs) noexcept
        {
            return lhs.base() != rhs.base();
        }

        const_reverse_iterator& operator++() noexcept
        {
            it_ += size_;
            return *this;
        }

        const_reverse_iterator& operator--() noexcept
        {
            it_ -= size_;
            return *this;
        }

        const_reverse_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        const_reverse_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        const_reverse_iterator& operator+=(difference_type movement) noexcept
        {
            it_ += (movement * size_);
            return *this;
        }

        const_reverse_iterator& operator-=(difference_type movement) noexcept
        {
            it_ -= (movement * size_);
            return *this;
        }

        const_reverse_iterator operator+(difference_type movement) const
            noexcept
        {
            auto r = *this;
            r += movement;
            return r;
        }

        const_reverse_iterator operator-(difference_type movement) const
            noexcept
        {
            auto r = *this;
            r -= movement;
            return r;
        }

        difference_type operator-(const const_reverse_iterator& other) const
            noexcept
        {
            assert(group_size() == other.group_size());
            assert((base() - other.base()) % group_size() == 0);

            return (base() - other.base()) / group_size();
        }

        reference operator*() const noexcept
        {
            return {*it_, *size_it_, size_};
        }
    };

    struct sentinel
    {
        using vector_const_sentinel_type =
            matter::const_sentinel_t<std::vector<matter::erased_storage>>;

    private:
        vector_const_sentinel_type sent_;

    public:
        constexpr sentinel() noexcept = default;

        constexpr sentinel(vector_const_sentinel_type sent) noexcept
            : sent_{sent}
        {}

        auto operator==(const const_iterator& cit) const noexcept
        {
            return sent_ == cit.base();
        }

        auto operator!=(const const_iterator& cit) const noexcept
        {
            return !(*this == cit);
        }

        friend auto operator==(const const_iterator& it,
                               const sentinel&       sent) noexcept
        {
            return it.base() == sent.sent_;
        }

        friend auto operator!=(const const_iterator& it,
                               const sentinel&       sent) noexcept
        {
            return !(it == sent);
        }

        auto operator-(const const_iterator& it) const noexcept
        {
            return (sent_ - it.base()) / it.group_size();
        }

        friend auto operator-(const const_iterator& it,
                              const sentinel&       sent) noexcept
        {
            return (it.base() - sent.sent_) / it.group_size();
        }
    };

    struct reverse_sentinel
    {
        using vector_const_reverse_sentinel_type =
            matter::const_reverse_sentinel_t<
                std::vector<matter::erased_storage>>;

    private:
        vector_const_reverse_sentinel_type sent_;

    public:
        constexpr reverse_sentinel() = default;

        constexpr reverse_sentinel(
            vector_const_reverse_sentinel_type sent) noexcept
            : sent_{sent}
        {}

        auto operator==(const reverse_iterator& rit) const noexcept
        {
            return sent_ == rit.base();
        }

        auto operator!=(const reverse_iterator& rit) const noexcept
        {
            return !(*this == rit);
        }

        friend auto operator==(const reverse_iterator& rit,
                               const reverse_sentinel& sent) noexcept
        {
            return sent == rit;
        }

        friend auto operator!=(const reverse_iterator& rit,
                               const reverse_sentinel& sent) noexcept
        {
            return !(rit == sent);
        }

        auto operator-(const reverse_iterator& rit) const noexcept
        {
            return (sent_ - rit.base()) / rit.group_size();
        }

        friend auto operator-(const reverse_iterator& rit,
                              const reverse_sentinel& sent) noexcept
        {
            return (rit.base() - sent.sent_) / rit.group_size();
        }
    };

public:
    using id_type = typename matter::erased_storage::id_type;

private:
    /// the number of components each group stores
    const std::size_t size_;
    /// contains the sizes of the groups, to not rely on the stores implementing
    /// size()
    std::vector<std::size_t>            group_sizes_{};
    std::vector<matter::erased_storage> groups_{};

public:
    explicit group_vector(std::size_t size) : size_{size}
    {}

    iterator begin() noexcept
    {
        return {groups_.begin(), group_sizes_.begin(), group_size()};
    }

    const_iterator begin() const noexcept
    {
        return {groups_.begin(), group_sizes_.begin(), group_size()};
    }

    auto end() const noexcept
    {
        return sentinel{groups_.cend()};
    }

    reverse_iterator rbegin() noexcept
    {
        // this arithmetic is necessary to properly align the offsets
        return reverse_iterator{groups_.rbegin() + (group_size() - 1),
                                group_sizes_.rbegin(),
                                group_size()};
    }

    auto rend() const noexcept
    {
        // make sure we correct the offset to that --~aaabbbccc becomes
        // ~--aaabbbccc, look for tilde (~)
        return reverse_sentinel{groups_.crend() + (group_size() - 1)};
    }

    std::size_t group_size() const noexcept
    {
        return size_;
    }

    template<typename... Ts>
    group<typename Ts::type...>
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
        auto grp         = *inserted_at;
        return group{ids, grp};
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

        auto grp = *it;
        return grp == ids ? it :
                            const_iterator{groups_.end(),
                                           group_sizes_.end(),
                                           group_size()};
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

        auto grp = *it;
        return grp == ids ?
                   it :
                   iterator{groups_.end(), group_sizes_.end(), group_size()};
    }

    template<typename... TIds>
    std::optional<group<typename TIds::type...>> find_group(
        const matter::unordered_typed_ids<id_type, TIds...>& ids,
        const matter::ordered_typed_ids<id_type, TIds...>& ordered_ids) noexcept
    {
        auto it = find(ordered_ids);

        if (it == end())
        {
            return {};
        }

        auto grp = *it;
        return group{ids, grp};
    }

    template<typename... TIds>
    std::optional<group<typename TIds::type...>> find_group(
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
        else if (auto grp = *it; !grp.contains(ordered_ids))
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
        return *it;
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
        return any_group{
            std::addressof(groups_[size_ * index]), group_sizes_[index], size_};
    }

    std::size_t size() const noexcept
    {
        return group_sizes_.size();
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
                auto grp = *pos;
                return !grp.contains(matter::ordered_typed_ids{unordered_ids});
            }
            return true;
        }()));

        auto distance = std::distance(begin(), pos);

        std::array<matter::erased_storage, sizeof...(TIds)> stores{
            matter::erased_storage{unordered_ids.template get<TIds>()}...};

        auto inserted_at =
            groups_.insert(pos.base(),
                           std::make_move_iterator(stores.begin()),
                           std::make_move_iterator(stores.end()));

        auto size_it = group_sizes_.insert(group_sizes_.begin() + distance, 0);

        std::sort(inserted_at, inserted_at + size_);
        return {inserted_at, size_it, group_size()};
    }

    template<typename... TIds>
    const_iterator
    lower_bound(const matter::ordered_typed_ids<id_type, TIds...>& ids) const
        noexcept
    {
        return matter::lower_bound(begin(), end(), ids);
    }

    template<typename... TIds>
    iterator
    lower_bound(const matter::ordered_typed_ids<id_type, TIds...>& ids) noexcept
    {
        return matter::lower_bound(begin(), end(), ids);
    }
};
} // namespace matter

#endif
