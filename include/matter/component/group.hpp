#ifndef MATTER_COMPONENT_GROUP_HPP
#define MATTER_COMPONENT_GROUP_HPP

#pragma once

#include <array>
#include <cassert>

#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/util/id_erased.hpp"

namespace matter
{
class group {
public:
    using id_type = typename matter::id_erased::id_type;

private:
    matter::id_erased* ptr_;
    const std::size_t  size_;

public:
    constexpr group(matter::id_erased* ptr, std::size_t size) noexcept
        : ptr_{ptr}, size_{size}
    {
        assert(is_sorted());
    }

    matter::id_erased* begin() noexcept
    {
        return ptr_;
    }

    const matter::id_erased* begin() const noexcept
    {
        return ptr_;
    }

    matter::id_erased* end() noexcept
    {
        return ptr_ + size_;
    }

    const matter::id_erased* end() const noexcept
    {
        return ptr_ + size_;
    }

    constexpr bool operator==(const group& other) const noexcept
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

    constexpr bool operator!=(const group& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const group& other) const noexcept
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

    constexpr bool operator>(const group& other) const noexcept
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

    template<typename C, id_type V>
    constexpr bool contains(const typed_id<id_type, C, V>& id) const noexcept
    {
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

    constexpr bool contains(const group& other) const noexcept
    {
        return *this == other;
    }

    template<typename C, id_type V>
    constexpr matter::component_storage_t<C>&
    storage(const typed_id<id_type, C, V>& id) noexcept
    {
        assert(contains(id));

        auto ptr = find_id(id);
        return ptr->template get<matter::component_storage_t<C>>();
    }

    template<typename C, id_type V>
    const matter::component_storage_t<C>&
    storage(const typed_id<id_type, C, V>& id) const noexcept
    {
        assert(contains(id));

        auto ptr = find_id(id);
        return ptr->template get<matter::component_storage_t<C>>();
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

    friend void swap(group& lhs, group& rhs) noexcept;

private:
    /// \brief elements in a group must always be sorted
    /// this function is a precondition for most operations to ensure all
    /// `id_erased` are always sorted, this is required for `std::includes`
    /// and to efficiently check whether ids are contained within this group
    bool is_sorted() const noexcept
    {
        return std::is_sorted(ptr_, ptr_ + size_);
    }

    template<typename C, id_type V>
    matter::id_erased* find_id(const typed_id<id_type, C, V>& id) noexcept
    {
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return end();
        }

        return it;
    }

    template<typename C, id_type V>
    const matter::id_erased* find_id(const typed_id<id_type, C, V>& id) const
        noexcept
    {
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return end();
        }

        return it;
    }
};

void swap(group& lhs, group& rhs) noexcept
{
    assert(lhs.size_ == rhs.size_);

    for (std::size_t i = 0; i < lhs.size_; ++i)
    {
        swap(lhs.ptr_[i], rhs.ptr_[i]);
    }
}
} // namespace matter

#endif
