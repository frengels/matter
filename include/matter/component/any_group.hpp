#ifndef MATTER_COMPONENT_ANY_GROUP_HPP
#define MATTER_COMPONENT_ANY_GROUP_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/storage/erased_storage.hpp"

namespace matter
{
namespace detail
{
/// \brief a group describes the containers for a tuple of components
/// A group is a lightweight construct which represents a slice of a
/// `group_vector`, the `erased_storage` contained in the group represent a
/// container for components identified by the `id` within `erased_storage`.
template<bool Const = false>
class any_group {
private:
    static constexpr auto is_const = Const;

    using erased_type     = std::conditional_t<is_const,
                                           const matter::erased_storage*,
                                           matter::erased_storage*>;
    using erased_type_ref = std::conditional_t<is_const,
                                               const matter::erased_storage&,
                                               matter::erased_storage&>;

    template<bool _Const>
    friend class any_group;

public:
    using id_type   = typename matter::erased_storage::id_type;
    using size_type = typename matter::erased_storage::size_type;

    using const_iterator = const matter::erased_storage*;
    using iterator =
        std::conditional_t<is_const, const_iterator, matter::erased_storage*>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;

private:
    erased_type ptr_;
    std::size_t size_;

public:
    constexpr any_group(erased_type ptr, std::size_t size) noexcept
        : ptr_{ptr}, size_{size}
    {
        assert(is_sorted());
    }

    constexpr any_group(erased_type_ref ref, std::size_t size) noexcept
        : any_group{std::addressof(ref), size}
    {}

    constexpr any_group(const any_group<false>& mutable_grp)
        : ptr_{mutable_grp.ptr_}, size_{mutable_grp.size_}
    {}

    iterator begin() noexcept
    {
        return ptr_;
    }

    iterator end() noexcept
    {
        return ptr_ + size_;
    }

    const_iterator begin() const noexcept
    {
        return ptr_;
    }

    const_iterator end() const noexcept
    {
        return ptr_ + size_;
    }

    const_iterator cbegin() const noexcept
    {
        return ptr_;
    }

    const_iterator cend() const noexcept
    {
        return ptr_;
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

    constexpr erased_type data() noexcept
    {
        return ptr_;
    }

    constexpr void erase(size_type idx) noexcept
    {
        std::for_each(begin(), end(), [&](auto&& erased_storage) {
            erased_storage.erase(idx);
        });
    }

    constexpr const matter::erased_storage* data() const noexcept
    {
        return ptr_;
    }

    constexpr bool operator==(const any_group& other) const noexcept
    {
        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] != other.ptr_[i])
            {
                return false;
            }
        }

        return true;
    }

    constexpr bool operator!=(const any_group& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename... TIds>
    constexpr auto
    operator==(const ordered_typed_ids<id_type, TIds...>& ids) const noexcept
    {
        assert(ids.size() == size());

        for (std::size_t i = 0; i < size(); ++i)
        {
            if (ptr_[i] != ids[i])
            {
                return false;
            }
        }

        return true;
    }

    template<typename... TIds>
    constexpr auto
    operator!=(const ordered_typed_ids<id_type, TIds...>& ids) const noexcept
    {
        return !(*this == ids);
    }

    constexpr bool operator<(const any_group& other) const noexcept
    {
        assert(size() == other.size());

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] < other.ptr_[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Ts>
    constexpr bool operator<(const ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() == size_);

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] < ids[i])
            {
                return true;
            }
        }

        return false;
    }

    constexpr bool operator>(const any_group& other) const noexcept
    {
        assert(size() == other.size());

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] > other.ptr_[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Ts>
    constexpr bool operator>(const ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() == size_);

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] > ids[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Stores, typename... InputIts>
    void _insert_back_impl(
        std::tuple<Stores&...> stores,
        std::pair<
            InputIts,
            InputIts>... it_pairs) noexcept((std::
                                                 is_nothrow_constructible_v<
                                                     typename std::
                                                         iterator_traits<
                                                             InputIts>::
                                                             value_type,
                                                     typename std::
                                                         iterator_traits<
                                                             InputIts>::
                                                             reference> &&
                                             ...))
    {
        static_assert(sizeof...(Stores) == sizeof...(InputIts));

        (
            [&]() {
                auto& store = std::get<Stores&>(stores);
                store.insert(store.end(), it_pairs.first, it_pairs.second);
            }(),
            ...);
    }

    template<typename... TIds, typename... InputIts>
    void insert_back(
        const matter::unordered_typed_ids<id_type, TIds...>& ids,
        std::pair<
            InputIts,
            InputIts>... it_pairs) noexcept((std::
                                                 is_nothrow_constructible_v<
                                                     typename std::
                                                         iterator_traits<
                                                             InputIts>::
                                                             value_type,
                                                     typename std::
                                                         iterator_traits<
                                                             InputIts>::
                                                             reference> &&
                                             ...))
    {
        static_assert(sizeof...(TIds) == sizeof...(InputIts),
                      "Not the same amount typed_ids and InputIts passed.");
        static_assert((std::is_same_v<typename TIds::type,
                                      typename InputIts::value_type> &&
                       ...),
                      "typed_id::type and InputIt::value_type do not match.");

        auto stores = storage(ids);
        _insert_back_impl(stores, it_pairs...);
    }

    template<typename... Cs,
             typename... Ts,
             typename... TupArgs,
             std::size_t... Is>
    void
    _emplace_back_impl(std::tuple<Ts&...> storages, TupArgs&&... tupargs) noexcept(
        (detail::is_nothrow_constructible_expand_tuple_v<Cs, TupArgs> && ...))
    {
        (std::get<Ts&>(storages).emplace_back(
             detail::construct_from_tuple<Cs>(std::forward<TupArgs>(tupargs))),
         ...);
    }

    template<typename... Ts, typename... TupArgs>
    void emplace_back(
        const unordered_typed_ids<id_type, Ts...>& ids,
        TupArgs&&... forw_cargs) noexcept((detail::
                                               is_nothrow_constructible_expand_tuple_v<
                                                   typename Ts::type,
                                                   TupArgs> &&
                                           ...))
    {
        static_assert(sizeof...(Ts) == sizeof...(TupArgs),
                      "not enough/too many Ts... or TupArgs...");
        static_assert(
            (detail::is_constructible_expand_tuple_v<typename Ts::type,
                                                     TupArgs> &&
             ...),
            "Cannot construct one of the components from passed args");

        // if this isn't true then we're emplacing in the wrong group
        assert(sizeof...(Ts) == size_);
        // if this isn't true then this group doesn't contain the ids
        assert(contains(ordered_typed_ids{ids}));

        auto storage_tup = storage(ids);

        _emplace_back_impl<typename Ts::type...>(
            storage_tup, std::forward<TupArgs>(forw_cargs)...);

        // prefer this function whenever it decides to work on clang
        // or also the new lambda template expansion syntax would hopefully work
        // on clang with ...args = something
        /*
        std::apply(
            [&](auto&... storage) {
                (storage.emplace_back(
                     detail::construct_from_tuple<typename Ts::type>(
                         std::forward<TupArgs>(forw_cargs))),
                 ...);
            },
            storage_tup);
        */
    }

    constexpr std::size_t size() const noexcept
    {
        return size_;
    }

    template<typename TId>
    constexpr std::enable_if_t<matter::is_typed_id_v<TId>, bool>
    contains(const TId& id) const noexcept
    {
        static_assert(matter::has_same_id_type_v<TId, id_type>);
        auto* ptr = find_id(id);
        return ptr == end() ? false : true;
    }

    template<typename... Ts>
    constexpr bool contains(const ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() <= size_);
        return std::includes(ptr_, ptr_ + size_, ids.begin(), ids.end());
    }

    constexpr bool contains(const any_group& other) const noexcept
    {
        return *this == other;
    }

    template<typename TId>
    constexpr std::enable_if_t<matter::is_typed_id_v<TId> &&
                                   matter::has_same_id_type_v<TId, id_type>,
                               matter::component_storage_t<typename TId::type>&>
    storage(const TId& id) noexcept
    {
        assert(contains(id));

        auto ptr = find_id(id);
        return ptr->template get<typename TId::type>();
    }

    template<typename TId>
    constexpr std::enable_if_t<
        matter::is_typed_id_v<TId> && matter::has_same_id_type_v<TId, id_type>,
        const matter::component_storage_t<typename TId::type>&>
    storage(const TId& id) const noexcept
    {
        assert(contains(id));

        auto ptr = find_id(id);
        return ptr->template get<typename TId::type>();
    }

    template<typename... Ts>
    std::tuple<matter::component_storage_t<typename Ts::type>&...>
    storage(const unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        assert(contains(ordered_typed_ids{ids}));
        assert(ids.size() <= size_);

        return {storage(ids.template get<Ts>())...};
    }

    template<typename... Ts>
    std::tuple<const matter::component_storage_t<typename Ts::type>&...>
    storage(const unordered_typed_ids<id_type, Ts...>& ids) const noexcept
    {
        assert(contains(ordered_typed_ids{ids}));
        assert(ids.size() <= size_);

        return {storage(ids.template get<Ts>())...};
    }

    friend void swap(any_group<Const>& lhs, any_group<Const>& rhs) noexcept
    {
        using std::swap;
        swap(lhs.ptr_, rhs.ptr_);
        swap(lhs.size_, rhs.size_);
    }

private:
    /// \brief elements in a group must always be sorted
    /// this function is a precondition for most operations to ensure all
    /// `erased_storage` are always sorted, this is required for `std::includes`
    /// and to efficiently check whether ids are contained within this group
    bool is_sorted() const noexcept
    {
        return std::is_sorted(ptr_, ptr_ + size_);
    }

    template<typename TId>
    constexpr std::enable_if_t<matter::is_typed_id_v<TId>,
                               matter::erased_storage*>
    find_id(const TId& id) noexcept
    {
        static_assert(matter::has_same_id_type_v<TId, id_type>);
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return end();
        }

        return it;
    }

    template<typename TId>
    constexpr std::enable_if_t<matter::is_typed_id_v<TId>,
                               const matter::erased_storage*>
    find_id(const TId& id) const noexcept
    {
        static_assert(matter::has_same_id_type_v<TId, id_type>);
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return end();
        }

        return it;
    }
};
} // namespace detail

using any_group = ::matter::detail::any_group<false>;

/// \brief a group where the contained stores cannot be modified
using const_any_group = ::matter::detail::any_group<true>;

} // namespace matter

#endif
