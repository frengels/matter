#ifndef MATTER_COMPONENT_GROUP_HPP
#define MATTER_COMPONENT_GROUP_HPP

#pragma once

#include <array>
#include <cassert>

#include "matter/component/traits.hpp"
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

    template<std::size_t N>
    constexpr bool operator<(const std::array<id_type, N>& sorted_ids) const
        noexcept
    {
        assert(N == size_);
        assert(std::is_sorted(sorted_ids.begin(), sorted_ids.end()));

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] < sorted_ids[i])
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

    template<std::size_t N>
    constexpr bool operator>(const std::array<id_type, N>& sorted_ids) const
        noexcept
    {
        assert(N == size_);
        assert(std::is_sorted(sorted_ids.begin(), sorted_ids.end()));

        for (std::size_t i = 0; i < size_; ++i)
        {
            if (ptr_[i] > sorted_ids[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Cs, std::size_t N, typename... TupArgs>
    void emplace_back(const std::array<id_type, N>& ids,
                      TupArgs&&... forw_cargs) noexcept
    {
        static_assert(sizeof...(Cs) == N, "not enough/too many Cs... or ids");
        static_assert(sizeof...(TupArgs) == N,
                      "not enough/too many TupArgs... or ids");
        static_assert(
            (detail::is_constructible_expand_tuple_v<Cs, TupArgs> && ...),
            "Cannot construct one of the components from passed args");

        // if this isn't true then we're emplacing in the wrong group
        assert(N == size_);
        // if this isn't true then this group doesn't contain the ids
        assert(contains_unsorted(ids));

        auto storage_tup = storage<Cs...>(ids);

        std::apply(
            [&](auto&... storage) {
                (storage.emplace_back(detail::construct_from_tuple(
                     std::in_place_type_t<Cs>{},
                     std::forward<TupArgs>(forw_cargs))),
                 ...);
            },
            storage_tup);
    }

    constexpr std::size_t size() const noexcept
    {
        return size_;
    }

    bool contains(id_type id) const noexcept
    {
        auto* it = find_id(id);

        return it == end() ? false : true;
    }

    template<std::size_t N>
    constexpr bool contains(const std::array<id_type, N>& ids) const noexcept
    {
        assert(N <= size_);
        return std::includes(ptr_, ptr_ + size_, ids.begin(), ids.end());
    }

    template<std::size_t N>
    constexpr bool contains_unsorted(const std::array<id_type, N>& ids) const
        noexcept
    {
        auto sorted_ids = ids;
        std::sort(sorted_ids.begin(), sorted_ids.end());
        return contains(sorted_ids);
    }

    constexpr bool contains(const group& other) const noexcept
    {
        return *this == other;
    }

    template<typename C>
    matter::component_storage_t<C>& storage(id_type id) noexcept
    {
        assert(contains(id));

        auto it = find_id(id);
        return it->get<matter::component_storage_t<C>>();
    }

    template<typename C>
    const matter::component_storage_t<C>& storage(id_type id) const noexcept
    {
        assert(contains(id));

        auto it = find_id(id);
        return it->get<matter::component_storage_t<C>>();
    }

    // retrieve the storages for the passed ids, assuming that those ids belong
    // to the passed component types
    template<typename... Cs, std::size_t N>
    std::tuple<matter::component_storage_t<Cs>&...>
    storage(const std::array<id_type, N>& ids) noexcept
    {
        return storage_impl<Cs...>(std::index_sequence_for<Cs...>{}, ids);
    }

private:
    template<typename... Cs, std::size_t... Is, std::size_t N>
    std::tuple<matter::component_storage_t<Cs>&...>
    storage_impl(std::index_sequence<Is...>,
                 const std::array<id_type, N>& ids) noexcept
    {
        static_assert(
            sizeof...(Cs) == N,
            "the amount of Cs... and the size of the id array must match");
        assert(contains_unsorted(ids));
        assert(N == size_);

        return {storage<Cs>(ids[Is])...};
    }

public:
    template<typename... Cs, std::size_t N>
    std::tuple<const matter::component_storage_t<Cs>&...>
    storage(const std::array<id_type, N>& ids) const noexcept
    {
        return storage_impl<Cs...>(std::index_sequence_for<Cs...>{}, ids);
    }

private:
    template<typename... Cs, std::size_t... Is, std::size_t N>
    std::tuple<const matter::component_storage_t<Cs>&...>
    storage_impl(std::index_sequence<Is...>,
                 const std::array<id_type, N>& ids) const noexcept
    {
        static_assert(
            sizeof...(Cs) == N,
            "the amount of Cs... and the size of the id array must match");
        assert(contains_unsorted(ids));
        assert(N < size_);

        return {storage<Cs>(ids[Is])...};
    }

public:
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

    matter::id_erased* find_id(id_type id) noexcept
    {
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || it->id() != id)
        {
            return end();
        }

        return it;
    }

    const matter::id_erased* find_id(id_type id) const noexcept
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
