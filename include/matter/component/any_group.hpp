#ifndef MATTER_COMPONENT_ANY_GROUP_HPP
#define MATTER_COMPONENT_ANY_GROUP_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/component/untyped_id.hpp"
#include "matter/storage/erased_storage.hpp"

namespace matter
{
namespace detail
{
/// \brief a group describes the containers for a tuple of components
/// A group is a lightweight construct which represents a slice of a
/// `group_vector`, the `erased_storage` contained in the group represent a
/// container for components identified by the `id` within `erased_storage`.
template<typename Id, bool Const = false>
class any_group {
public:
    using id_type = Id;

private:
    static constexpr auto is_const = Const;

    using erased_type =
        std::conditional_t<is_const,
                           const matter::erased_storage<id_type>*,
                           matter::erased_storage<id_type>*>;
    using erased_type_ref =
        std::conditional_t<is_const,
                           const matter::erased_storage<id_type>&,
                           matter::erased_storage<id_type>&>;

    friend class any_group<Id, true>;

public:
    using size_type = typename matter::erased_storage<id_type>::size_type;

    using const_iterator = const matter::erased_storage<id_type>*;
    using iterator       = std::conditional_t<is_const,
                                        const_iterator,
                                        matter::erased_storage<id_type>*>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;

private:
    erased_type ptr_;
    std::size_t group_size_;

public:
    constexpr any_group() noexcept = default;

    constexpr any_group(erased_type ptr, std::size_t group_size) noexcept
        : ptr_{ptr}, group_size_{group_size}
    {
        // when explicitly initialized group_size must be over 0
        assert(group_size > 0);
        assert(is_sorted());
    }

    constexpr any_group(erased_type_ref ref, std::size_t size) noexcept
        : any_group{std::addressof(ref), size}
    {}

    constexpr any_group(const any_group<id_type, false>& mutable_grp)
        : ptr_{mutable_grp.ptr_}, group_size_{mutable_grp.group_size_}
    {}

    iterator begin() noexcept
    {
        return ptr_;
    }

    iterator end() noexcept
    {
        return ptr_ + group_size();
    }

    const_iterator begin() const noexcept
    {
        return ptr_;
    }

    const_iterator end() const noexcept
    {
        return ptr_ + group_size();
    }

    const_iterator cbegin() const noexcept
    {
        return ptr_;
    }

    const_iterator cend() const noexcept
    {
        return ptr_ + group_size();
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

    constexpr const erased_type data() const noexcept
    {
        return ptr_;
    }

    constexpr explicit operator bool() const noexcept
    {
        return data();
    }

    constexpr void erase(size_type idx) noexcept
    {
        std::for_each(begin(), end(), [&](auto&& erased_storage) {
            erased_storage.erase(idx);
        });
    }

    /// get the object with the passed id at the passed index
    constexpr erased_component<id_type> get_at(const id_type& id,
                                               size_type      index) noexcept
    {
        assert(contains(id));
        auto* storage = find_id(id);

        return storage[index];
    }

    constexpr void push_back(erased_component<id_type> comp) noexcept
    {
        auto* storage = find_id(comp.id());
        assert(storage);

        storage->push_back(comp);
    }

    /// \brief used to validate that all underlying sizes are equal
    constexpr bool are_sizes_valid() const noexcept
    {
        auto expected_sz = ptr_->size();

        for (std::size_t i = 1; i < group_size(); ++i)
        {
            if (ptr_[i].size() != expected_sz)
            {
                return false;
            }
        }

        return true;
    }

    constexpr bool operator==(const any_group& other) const noexcept
    {
        for (std::size_t i = 0; i < group_size(); ++i)
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

    constexpr auto operator==(matter::ordered_untyped_ids<id_type> ids) const
        noexcept
    {
        assert(ids.size() == group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] != ids[i])
            {
                return false;
            }
        }

        return true;
    }

    template<typename... Ts>
    constexpr auto
    operator==(const ordered_typed_ids<id_type, Ts...>& ids) const noexcept
    {
        assert(ids.size() == group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] != ids[i])
            {
                return false;
            }
        }

        return true;
    }

    constexpr auto operator!=(matter::ordered_untyped_ids<id_type> ids) const
        noexcept
    {
        return !(*this == std::move(ids));
    }

    template<typename... Ts>
    constexpr auto
    operator!=(const matter::ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        return !(*this == ids);
    }

    constexpr bool operator<(const any_group& other) const noexcept
    {
        assert(group_size() == other.group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] < other.ptr_[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Ts>
    constexpr bool
    operator<(const matter::ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() == group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] < ids[i])
            {
                return true;
            }
        }

        return false;
    }

    constexpr auto operator<(matter::ordered_untyped_ids<id_type> ids) const
        noexcept
    {
        assert(group_size() == ids.size());

        for (std::size_t i = 0; i < group_size(); ++i)
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
        assert(group_size() == other.group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] > other.ptr_[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Ts>
    constexpr bool
    operator>(const matter::ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() == group_size());

        for (std::size_t i = 0; i < group_size(); ++i)
        {
            if (ptr_[i] > ids[i])
            {
                return true;
            }
        }

        return false;
    }

    constexpr auto operator>(matter::ordered_untyped_ids<id_type> ids) const
        noexcept
    {
        assert(group_size() == ids.size());

        for (std::size_t i = 0; i < group_size(); ++i)
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

    template<typename... Ts, typename... InputIts>
    void insert_back(
        const matter::unordered_typed_ids<id_type, Ts...>& ids,
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
        static_assert(sizeof...(Ts) == sizeof...(InputIts),
                      "Not the same amount typed_ids and InputIts passed.");
        static_assert(
            (std::is_same_v<Ts, typename InputIts::value_type> && ...),
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
        const matter::unordered_typed_ids<id_type, Ts...>& ids,
        TupArgs&&... forw_cargs) noexcept((detail::
                                               is_nothrow_constructible_expand_tuple_v<
                                                   typename Ts::type,
                                                   TupArgs> &&
                                           ...))
    {
        static_assert(sizeof...(Ts) == sizeof...(TupArgs),
                      "not enough/too many Ts... or TupArgs...");
        static_assert(
            (detail::is_constructible_expand_tuple_v<Ts, TupArgs> && ...),
            "Cannot construct one of the components from passed args");

        // if this isn't true then we're emplacing in the wrong group
        assert(sizeof...(Ts) == group_size());
        // if this isn't true then this group doesn't contain the ids
        assert(contains(ordered_typed_ids{ids}));

        auto storage_tup = storage(ids);

        _emplace_back_impl<Ts...>(storage_tup,
                                  std::forward<TupArgs>(forw_cargs)...);

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
        assert(are_sizes_valid());
        return ptr_->size();
    }

    constexpr std::size_t group_size() const noexcept
    {
        return group_size_;
    }

    constexpr bool contains(const id_type& id) const noexcept
    {
        auto* ptr = find_id(id);
        return ptr;
    }

    template<typename... Ts>
    constexpr bool contains(const ordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(ids.size() <= group_size());
        return std::includes(ptr_, ptr_ + group_size(), ids.begin(), ids.end());
    }

    constexpr bool contains(const any_group& other) const noexcept
    {
        return *this == other;
    }

    template<typename T>
    constexpr matter::component_storage_t<T>&
    storage(const matter::typed_id<id_type, T>& tid) noexcept
    {
        assert(contains(tid));

        auto ptr = find_id(tid);
        return ptr->template get<T>();
    }

    template<typename T>
    constexpr

        const matter::component_storage_t<T>&
        storage(const matter::typed_id<id_type, T>& tid) const noexcept
    {
        assert(contains(tid));

        auto ptr = find_id(tid);
        return ptr->template get<T>();
    }

    template<typename... Ts>
    std::tuple<matter::component_storage_t<Ts>&...>
    storage(const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        assert(contains(ordered_typed_ids{ids}));
        assert(ids.size() <= group_size());

        return {storage(ids.template get<Ts>())...};
    }

    template<typename... Ts>
    std::tuple<const matter::component_storage_t<Ts>&...>
    storage(const matter::unordered_typed_ids<id_type, Ts...>& ids) const
        noexcept
    {
        assert(contains(ordered_typed_ids{ids}));
        assert(ids.size() <= group_size());

        return {storage(ids.template get<Ts>())...};
    }

    friend void swap(any_group<id_type, Const>& lhs,
                     any_group<id_type, Const>& rhs) noexcept
    {
        using std::swap;
        swap(lhs.ptr_, rhs.ptr_);
        swap(lhs.size_, rhs.size_);
    }

    erased_type find_id(const id_type& id) noexcept
    {
        auto it = matter::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return nullptr;
        }

        return it;
    }

    const matter::erased_storage<id_type>* find_id(const id_type& id) const
        noexcept
    {
        auto it = matter::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return nullptr;
        }

        return it;
    }

private:
    /// \brief elements in a group must always be sorted
    /// this function is a precondition for most operations to ensure all
    /// `erased_storage` are always sorted, this is required for `std::includes`
    /// and to efficiently check whether ids are contained within this group
    bool is_sorted() const noexcept
    {
        return std::is_sorted(begin(), end());
    }
};
} // namespace detail

template<typename Id>
using any_group = ::matter::detail::any_group<Id, false>;

/// \brief a group where the contained stores cannot be modified
template<typename Id>
using const_any_group = ::matter::detail::any_group<Id, true>;
} // namespace matter

#endif
