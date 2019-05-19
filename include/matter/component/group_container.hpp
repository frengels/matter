#ifndef MATTER_COMPONENT_GROUP_VECTOR_CONTAINER_HPP
#define MATTER_COMPONENT_GROUP_VECTOR_CONTAINER_HPP

#pragma once

#include <range/v3/view/all.hpp>

#include "matter/component/any_group.hpp"
#include "matter/component/group.hpp"
#include "matter/component/typed_id.hpp"

namespace matter
{
// a subset of the group_vector_container
template<typename Id>
class sized_group_range {
    static_assert(matter::is_id_v<Id>);

public:
    using id_type = Id;

    using erased_type = typename matter::any_group<id_type>::erased_type;
    using underlying_iterator_type =
        matter::iterator_t<std::vector<erased_type>>;

    class sentinel;

    class iterator {
        friend class sentinel;

        using underlying_iterator_type =
            matter::iterator_t<std::vector<erased_type>>;

    public:
        using value_type        = matter::any_group<id_type>;
        using pointer           = void;
        using reference         = matter::any_group<id_type>;
        using iterator_category = typename std::iterator_traits<
            typename std::vector<erased_type>::iterator>::iterator_category;
        using difference_type = typename std::iterator_traits<
            typename std::vector<erased_type>::iterator>::difference_type;

    private:
        std::size_t              grp_size_{};
        underlying_iterator_type it_;

    public:
        constexpr iterator() noexcept = default;

        constexpr iterator(underlying_iterator_type it, std::size_t grp_size)
            : grp_size_{grp_size}, it_{it}
        {}

        constexpr std::size_t group_size() const noexcept
        {
            return grp_size_;
        }

        constexpr underlying_iterator_type base() const noexcept
        {
            return it_;
        }

        constexpr bool operator==(const underlying_iterator_type& it) const
            noexcept
        {
            return it == it_;
        }

        constexpr bool operator!=(const underlying_iterator_type& it) const
            noexcept
        {
            return !(*this == it);
        }

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return it_ == other.it_;
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr iterator& operator++() noexcept
        {
            it_ += group_size();
            return *this;
        }

        constexpr iterator& operator--() noexcept
        {
            it_ -= group_size();
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr iterator& operator+=(difference_type movement) noexcept
        {
            it_ += (group_size() * movement);
            return *this;
        }

        constexpr iterator& operator-=(difference_type movement) noexcept
        {
            it_ -= (group_size() * movement);
            return *this;
        }

        constexpr iterator operator+(difference_type movement) const noexcept
        {
            auto tmp = *this;
            tmp += movement;
            return tmp;
        }

        constexpr iterator operator-(difference_type movement) const noexcept
        {
            auto tmp = *this;
            tmp -= movement;
            return tmp;
        }

        difference_type operator-(const iterator& other) const noexcept
        {
            return (it_ - other.it_) / group_size();
        }

        reference operator*() noexcept
        {
            return matter::any_group<id_type>{std::addressof(*it_),
                                              group_size()};
        }
    };

    class sentinel {
        using underlying_sentinel_type =
            matter::const_sentinel_t<std::vector<erased_type>>;

    private:
        underlying_sentinel_type sent_;

    public:
        constexpr sentinel() noexcept = default;

        explicit constexpr sentinel(underlying_sentinel_type sent) : sent_{sent}
        {}

        typename iterator::difference_type operator-(const iterator& it) const
            noexcept
        {
            return (sent_ - it.it_) / it.group_size();
        }

        friend typename iterator::difference_type
        operator-(const iterator& it, const sentinel& sent) noexcept
        {
            return (it.it_ - sent.sent_) / it.group_size();
        }

        bool operator==(const iterator& other) const noexcept
        {
            return sent_ == other.it_;
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        bool operator==(const sentinel& other) const noexcept
        {
            return sent_ == other.sent_;
        }

        bool operator!=(const sentinel& other) const noexcept
        {
            return !(*this == other);
        }

        bool operator==(const underlying_sentinel_type& sent) const noexcept
        {
            return sent_ == sent;
        }

        bool operator!=(const underlying_sentinel_type& sent) const noexcept
        {
            return !(*this == sent);
        }

        friend bool operator==(const iterator& it,
                               const sentinel& sent) noexcept
        {
            return sent == it;
        }

        friend bool operator!=(const iterator& it,
                               const sentinel& sent) noexcept
        {
            return sent != it;
        }

        friend bool operator==(const underlying_sentinel_type& sent,
                               const iterator&                 it) noexcept
        {
            return it == sent;
        }

        friend bool operator!=(const underlying_sentinel_type& sent,
                               const iterator&                 it) noexcept
        {
            return it != sent;
        }
    };

private:
    std::size_t              grp_size_{0};
    underlying_iterator_type begin_;
    underlying_iterator_type end_;

public:
    constexpr sized_group_range() noexcept
    {}

    constexpr sized_group_range(std::size_t              group_size,
                                underlying_iterator_type beg,
                                underlying_iterator_type end) noexcept
        : grp_size_{group_size}, begin_{beg}, end_{end}
    {}

    constexpr iterator begin() noexcept
    {
        return {begin_, grp_size_};
    }

    sentinel end() const noexcept
    {
        return sentinel{end_};
    }

private:
    constexpr iterator iterator_end() noexcept
    {
        return {end_, grp_size_};
    }

public:
    std::size_t group_size() const noexcept
    {
        return grp_size_;
    }

    std::size_t size() const noexcept
    {
        return sentinel{end_} - iterator{begin_, grp_size_};
    }

    template<typename... Ts>
    constexpr iterator
    find(const matter::ordered_typed_ids<id_type, Ts...>& ordered_ids) noexcept
    {
        assert(group_size() == ordered_ids.size());
        auto it = lower_bound(ordered_ids);

        if (it == end() || *it != ordered_ids)
        {
            // return the end in iterator form
            return iterator_end();
        }

        // we found it!
        return it;
    }

    constexpr iterator
    find(const matter::ordered_untyped_ids<id_type> ordered_ids) noexcept
    {
        assert(group_size() == ordered_ids.size());
        auto it = lower_bound(ordered_ids);

        if (it == end() || *it != ordered_ids)
        {
            return iterator_end();
        }

        return it;
    }

    // basically a binary search
    template<typename... Ts>
    constexpr iterator lower_bound(
        const matter::ordered_typed_ids<id_type, Ts...>& ordered_ids) noexcept
    {
        assert(ordered_ids.size() == group_size());
        return matter::lower_bound(begin(), end(), ordered_ids);
    }

    constexpr iterator
    lower_bound(const matter::ordered_untyped_ids<id_type> ordered_ids) noexcept
    {
        assert(group_size() == ordered_ids.size());
        return matter::lower_bound(begin(), end(), ordered_ids);
    }
};

template<typename Id>
class group_container {
    static_assert(matter::is_id_v<Id>);

public:
    using id_type     = Id;
    using erased_type = typename matter::any_group<id_type>::erased_type;

    using iterator       = matter::iterator_t<std::vector<erased_type>>;
    using const_iterator = matter::const_iterator_t<std::vector<erased_type>>;

private:
    // groups are made from erased_storage so we store those
    std::vector<erased_type> stores_;
    // this buffer is used to construct stores using untyped_ids, usually an
    // array is used but with untyped_ids the size isn't known at compile time
    // so we need dynamic storage. To not constantly allocate new storage we
    // have a central buffer for this. No mutating operations involving creating
    // groups can happen in parallel so this vector does not need to be thread
    // safe.
    std::vector<erased_type> stores_buffer_;

    // stores the begin index of each group size, inevitably to remain
    // consistent the start of group_size 1 which is always 0 will still be
    // stored in index = 0.
    std::vector<std::size_t> begin_indices_;

    // stores views to all groups managed by this group_container.
    // This cache is required to allow efficient iteration over all groups in
    // the container. Iterating otherwise would require conditional checks on
    // each increment of the iterator because we don't know when the group_size
    // might change.
    std::vector<matter::any_group<id_type>> view_cache_;

public:
    group_container() noexcept = default;

    constexpr iterator begin() noexcept
    {
        return stores_.begin();
    }

    constexpr iterator end() noexcept
    {
        return stores_.end();
    }

    constexpr const_iterator begin() const noexcept
    {
        return stores_.begin();
    }

    constexpr const_iterator end() const noexcept
    {
        return stores_.end();
    }

    constexpr std::size_t size() const noexcept
    {
        return stores_.size();
    }

    /// return the maximum group size, this can be used to iterate over all
    /// contained groups in combination with range(size_t)
    constexpr std::size_t groups_size() const noexcept
    {
        return begin_indices_.size();
    }

    constexpr auto range() noexcept
    {
        return ranges::view::all(view_cache_);
    }

    constexpr sized_group_range<id_type> range(std::size_t grp_size) noexcept
    {
        auto begin_index_it = begin_index_iterator(grp_size);
        auto end_index_it   = end_index_iterator(grp_size);

        if (begin_index_it >= begin_indices_.end())
        {
            return sized_group_range<id_type>{grp_size, end(), end()};
        }

        else if (end_index_it == begin_indices_.end())
        {
            return sized_group_range<id_type>{
                grp_size, stores_.begin() + *begin_index_it, stores_.end()};
        }

        else
        {
            return sized_group_range<id_type>{grp_size,
                                              stores_.begin() + *begin_index_it,
                                              stores_.begin() + *end_index_it};
        }
    }

    template<typename... Ts>
    constexpr std::optional<matter::group<id_type, Ts...>> find_group(
        const matter::unordered_typed_ids<id_type, Ts...>& ids,
        const matter::ordered_typed_ids<id_type, Ts...>&   ordered_ids) noexcept
    {
        // use the range's find to remove an if statement
        auto rng = range(ids.size());

        auto it = rng.find(ordered_ids);

        if (it != rng.end())
        {
            return matter::group{*it, ids};
        }

        return std::nullopt;
    }

    template<typename... Ts>
    constexpr std::optional<matter::group<id_type, Ts...>>
    find_group(const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return find_group(ids, matter::ordered_typed_ids{ids});
    }

    template<typename... Ts>
    constexpr typename sized_group_range<id_type>::iterator
    find(const matter::ordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        auto group_size = ids.size();
        auto rng        = range(group_size);

        auto it = rng.find(ids);

        if (it != rng.end())
        {
            // if not at the end we found a valid iterator, return it
            return it;
        }
        // return the full end of the container, past the end of the range.
        // this is for comparison with group_container::end().
        return {end(), group_size};
    }

    constexpr typename sized_group_range<id_type>::iterator
    find(const matter::ordered_untyped_ids<id_type> ids) noexcept
    {
        auto group_size = ids.size();
        auto rng        = range(group_size);

        auto it = rng.find(ids);

        if (it != rng.end())
        {
            return it;
        }

        return {end(), group_size};
    }

    constexpr std::optional<matter::any_group<id_type>>
    find_group(const matter::ordered_untyped_ids<id_type> ids) noexcept
    {
        auto group_size = ids.size();
        auto rng        = range(group_size);

        auto it = rng.find(ids);

        if (it != rng.end())
        {
            return *it;
        }

        return std::nullopt;
    }

    template<typename... Ts>
    constexpr typename sized_group_range<id_type>::iterator try_emplace(
        const matter::unordered_typed_ids<id_type, Ts...>& ids,
        const matter::ordered_typed_ids<id_type, Ts...>&   ordered_ids) noexcept
    {
        auto grp_size = ids.size();

        auto rng = range(grp_size);

        auto it = rng.lower_bound(ordered_ids);

        // if it is end then the appropriate group couldn't be found, insert
        if (it == rng.end() || *it != ordered_ids)
        {
            // set to the returned iterator
            it = emplace(it, ids);
        }

        return it;
    }

    /// emplace the group if it does not yet exist and return the iterator to
    /// this group, otherwise simply return the iterator to the requested group
    template<typename... Ts>
    constexpr typename sized_group_range<id_type>::iterator
    try_emplace(const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return try_emplace(ids, matter::ordered_typed_ids{ids});
    }

    /// try to emplace the group if it doesn't already exist using the vtable
    /// stored in the provided group. if already present, simply return
    constexpr typename sized_group_range<id_type>::iterator
    try_emplace(const matter::const_any_group<id_type>     storage_source,
                const matter::ordered_untyped_ids<id_type> copy_ids) noexcept
    {
        auto new_group_size = copy_ids.size();

        auto new_rng = range(new_group_size);
        auto it      = new_rng.lower_bound(copy_ids);

        if (it != new_rng.end() && *it == copy_ids)
        {
            // we found a valid group, return it
            return it;
        }
        else
        {
            // we can legally insert at the range end as well
            return emplace(it, storage_source, copy_ids);
        }
    }

    /// retrieves the iterator using try_emplace and constructs the correct
    /// group from it.
    /// \sa try_emplace for the used iterator
    template<typename... Ts>
    constexpr matter::group<id_type, Ts...> try_emplace_group(
        const matter::unordered_typed_ids<id_type, Ts...>& ids,
        const matter::ordered_typed_ids<id_type, Ts...>&   ordered_ids) noexcept
    {
        auto it = try_emplace(ids, ordered_ids);
        return matter::group{*it, ids};
    }

    /// \sa same as try_emplace but will construct ordered_ids from the
    /// unordered ones.
    template<typename... Ts>
    constexpr matter::group<id_type, Ts...> try_emplace_group(
        const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return try_emplace_group(ids, matter::ordered_typed_ids{ids});
    }

    constexpr matter::any_group<id_type>
    try_emplace_group(const matter::const_any_group<id_type>     store_source,
                      const matter::ordered_untyped_ids<id_type> ids) noexcept
    {
        return *try_emplace(store_source, ids);
    }

private:
    template<typename... Ts>
    constexpr typename sized_group_range<id_type>::iterator
    emplace(typename matter::sized_group_range<id_type>::iterator pos,
            const matter::unordered_typed_ids<id_type, Ts...>&    ids) noexcept

    {
        constexpr auto grp_size = sizeof...(Ts);
        assert(pos.group_size() == grp_size);

        // assert that the ids don't already exist
        // and the element at the position is greater than our ids.
        assert([&] {
            if (pos != end())
            {
                auto ordered_ids = matter::ordered_typed_ids{ids};
                return *pos != ordered_ids && *pos > ordered_ids;
            }

            return true;
        }());

        // create our stores, and sort them afterwards
        std::array<erased_type, grp_size> stores{
            matter::erased_storage{ids.template get<Ts>()}...};
        matter::insertion_sort(stores.begin(), stores.end());

        // create more indices if our current vector is too small
        resize_indices(grp_size);

        // iterator could invalidate so store the result
        auto new_pos = stores_.insert(pos.base(),
                                      std::make_move_iterator(stores.begin()),
                                      std::make_move_iterator(stores.end()));

        // increment all following index values
        increment_indices(grp_size);

        view_cache_.push_back({*new_pos, grp_size});

        // return where the inserted element is at
        return {new_pos, grp_size};
    }

    constexpr typename sized_group_range<id_type>::iterator
    emplace(typename matter::sized_group_range<id_type>::iterator pos,
            const matter::const_any_group<id_type>     storage_source,
            const matter::ordered_untyped_ids<id_type> copy_ids) noexcept
    {
        assert(pos.group_size() == copy_ids.size());
        assert(stores_buffer_.empty());
        assert(storage_source.contains(copy_ids));
        assert([&] {
            auto rng = range(copy_ids.size());
            if (pos != rng.end())
            {
                return *pos > copy_ids;
            }

            return true;
        }());

        auto grp_size = copy_ids.size();

        auto base_pos = pos.base();

        auto id_it = copy_ids.begin();

        resize_indices(grp_size);

        // make sure we have the space without having reallocs
        stores_buffer_.reserve(grp_size);

        // in the following we fill the end and move it to the correct position,
        // this is done to avoid many unnecessary moves of inserting single
        // stores.

        // fill the end of our vector with stores
        auto store_src_end = storage_source.end();
        for (auto store_it = storage_source.begin();
             store_it != store_src_end && id_it != copy_ids.end();
             ++store_it)
        {
            if (*store_it != *id_it)
            {
                // go to the next storage if this doesn't match
                continue;
            }
            else
            {
                stores_buffer_.emplace_back(store_it->duplicate_storage());
                ++id_it; // found it, let's move on
            }
        }

        // verify we have all the elements in our buffer
        assert(stores_buffer_.size() == grp_size);

        auto move_beg = stores_buffer_.begin();
        auto move_end = stores_buffer_.end();

        // insert them into their appropriate position
        auto new_pos = stores_.insert(base_pos,
                                      std::make_move_iterator(move_beg),
                                      std::make_move_iterator(move_end));

        // erase our temporary buffer
        stores_buffer_.clear();

        // increment all begin indices to match up again
        increment_indices(grp_size);

        return {new_pos, grp_size};
    }

    /// increment indices after the current, this method is called when a
    /// group was inserted
    constexpr void increment_indices(std::size_t group_size) noexcept
    {
        std::for_each(end_index_iterator(group_size),
                      begin_indices_.end(),
                      [&](auto& index) { index += group_size; });
    }

    /// adjust the size of the index vector, resize it when a group bigger than
    /// we currently account for is inserted
    constexpr void resize_indices(std::size_t group_size) noexcept
    {
        if (auto it = begin_index_iterator(group_size);
            it >= begin_indices_.end())
        {
            auto new_size = it - begin_indices_.begin() + 1;
            begin_indices_.resize(new_size, stores_.size());
        }
    }

    constexpr auto begin_index_iterator(std::size_t group_size) noexcept
    {
        return begin_indices_.begin() + (group_size - 1);
    }

    constexpr auto begin_index_iterator(std::size_t group_size) const noexcept
    {
        return begin_indices_.begin() + (group_size - 1);
    }

    constexpr auto end_index_iterator(std::size_t group_size) noexcept
    {
        return begin_indices_.begin() + group_size;
    }

    constexpr auto end_index_iterator(std::size_t group_size) const noexcept
    {
        return begin_indices_.begin() + group_size;
    }
};
} // namespace matter

#endif
