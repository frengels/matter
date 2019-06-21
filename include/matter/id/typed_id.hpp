#ifndef MATTER_COMPONENT_TYPED_ID_HPP
#define MATTER_COMPONENT_TYPED_ID_HPP

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "matter/id/id.hpp"
#include "matter/util/algorithm.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Id, typename... Ts>
class unordered_typed_ids;

template<typename T, typename Id, typename = void>
struct has_same_id_type : std::false_type
{};

template<typename T, typename Id>
struct has_same_id_type<
    T,
    Id,
    std::enable_if_t<std::is_same_v<typename T::id_type, Id>>> : std::true_type
{};

template<typename TId, typename Id>
constexpr auto has_same_id_type_v = has_same_id_type<TId, Id>::value;

template<typename TId, typename = void>
struct is_typed_id : std::false_type
{};

template<typename TId>
struct is_typed_id<
    TId,
    std::enable_if_t<matter::is_id_v<TId>, std::void_t<typename TId::type>>>
    : std::true_type
{};

template<typename T>
constexpr bool is_typed_id_v = is_typed_id<T>::value;

namespace impl
{
template<typename T, typename... TTs>
struct common_typed_id_type
{};

template<typename T, typename U, typename TT, typename... TTs>
struct common_typed_id_type_impl : common_typed_id_type<T, TTs...>
{};

template<typename T, typename TT, typename... TTs>
struct common_typed_id_type_impl<T, T, TT, TTs...>
{
    using type = TT;
};

template<typename T, typename TT, typename... TTs>
struct common_typed_id_type<T, TT, TTs...>
    : common_typed_id_type_impl<T, typename TT::type, TT, TTs...>
{};
} // namespace impl

template<typename T, typename... TIds>
struct common_typed_id_type : impl::common_typed_id_type<T, TIds...>
{};

template<typename T, typename... TIds>
using common_typed_id_type_t = typename common_typed_id_type<T, TIds...>::type;

template<typename Id, typename T>
class typed_id : public Id {
public:
    using type = T;

    using base_type = Id;

    using Id::Id;

    constexpr typed_id(const Id& id) noexcept : Id{id}
    {}

    constexpr const base_type& base() const noexcept
    {
        return *this;
    }

    constexpr base_type& base() noexcept
    {
        return *this;
    }

    constexpr bool operator==(const typed_id&) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const typed_id& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename U>
    constexpr bool operator==(const typed_id<Id, U>&) const noexcept
    {
        return false;
    }

    template<typename U>
    constexpr bool operator!=(const typed_id<Id, U>& other) const noexcept
    {
        return !(*this == other);
    }
};

static_assert(
    matter::is_typed_id_v<matter::typed_id<matter::signed_id<int>, int>>);

template<typename Id, typename... Ts>
class ordered_typed_ids;

// id the is the id_type to use and Ts... are the types represented by the
// stored ids.
template<typename Id, typename... Ts>
class unordered_typed_ids {
    static_assert(sizeof...(Ts) >= 1, "Cannot be empty");
    static_assert(matter::is_id_v<Id>,
                  "Id must be valid under the is_id concept");

    friend class ordered_typed_ids<Id, Ts...>;

    static constexpr auto size_ = sizeof...(Ts);

public:
    using id_type = Id;

private:
    std::array<id_type, sizeof...(Ts)> ids_;

public:
    constexpr unordered_typed_ids() noexcept : ids_{}
    {}

    constexpr unordered_typed_ids(const matter::typed_id<id_type, Ts>&... tids)
        : ids_{tids...}
    {}

    constexpr unordered_typed_ids(const Ts&... tids) noexcept : ids_{tids...}
    {}

    constexpr auto begin() const noexcept
    {
        return std::begin(ids_);
    }

    constexpr auto end() const noexcept
    {
        return std::end(ids_);
    }

    constexpr auto cbegin() const noexcept
    {
        return std::cbegin(ids_);
    }

    constexpr auto cend() const noexcept
    {
        return std::cend(ids_);
    }

    constexpr const id_type* data() const noexcept
    {
        return std::data(ids_);
    }

    constexpr auto size() const noexcept
    {
        return std::size(ids_);
    }

    explicit constexpr operator bool() const noexcept
    {
        return (bool(get<Ts>()) && ...);
    }

    constexpr const id_type& operator[](std::size_t index) const noexcept
    {
        assert(index < sizeof...(Ts));
        return ids_[index];
    }

    template<typename T>
    constexpr auto get() const noexcept
    {
        constexpr auto opt_index = detail::type_index<T, Ts...>();
        static_assert(bool(opt_index), "T is not contained in Ts...");

        constexpr auto index = *opt_index;

        return this->template get<index>();
    }

    template<std::size_t I>
    constexpr auto get() const noexcept
    {
        const id_type& tid = std::get<I>(ids_);
        assert(bool(tid));
        return matter::typed_id<id_type, detail::nth_t<I, Ts...>>{tid};
    }

    constexpr const std::array<id_type, sizeof...(Ts)> array() const noexcept
    {
        return ids_;
    }
};

template<typename Id, typename... Ts>
unordered_typed_ids(const matter::typed_id<Id, Ts>&... tids) noexcept
    ->unordered_typed_ids<Id, Ts...>;

template<typename Id, typename... Ts>
class ordered_typed_ids {
    static_assert(sizeof...(Ts) >= 1, "Cannot be empty");
    static_assert(matter::is_id_v<Id>,
                  "Id must be valid under the is_id concept");

public:
    using id_type = Id;

    static constexpr auto size_ = sizeof...(Ts);

private:
    std::array<id_type, sizeof...(Ts)> ordered_ids_{};

public:
    /// \brief default initialize ordered ids into an invalid state
    constexpr ordered_typed_ids() noexcept = default;

    constexpr ordered_typed_ids(
        const matter::typed_id<id_type, Ts>&... tids) noexcept
        : ordered_ids_{[&]() {
              std::array<id_type, sizeof...(Ts)> arr{tids...};
              matter::static_sort(arr);
              return arr;
          }()}
    {}

    constexpr ordered_typed_ids(
        const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
        : ordered_ids_{[&]() {
              auto arr = ids.array();
              matter::static_sort(arr);
              return arr;
          }()}
    {}

    explicit constexpr operator bool() const noexcept
    {
        return std::all_of(begin(), end(), [](auto&& id) { return bool(id); });
    }

    template<std::size_t N>
    constexpr const id_type& get() const noexcept
    {
        return std::get<N>(ordered_ids_);
    }

    constexpr auto begin() const noexcept
    {
        return std::begin(ordered_ids_);
    }

    constexpr auto end() const noexcept
    {
        return std::end(ordered_ids_);
    }

    constexpr auto cbegin() const noexcept
    {
        return std::cbegin(ordered_ids_);
    }

    constexpr auto cend() const noexcept
    {
        return std::cend(ordered_ids_);
    }

    constexpr const id_type* data() const noexcept
    {
        return std::data(ordered_ids_);
    }

    constexpr std::size_t size() const noexcept
    {
        return std::size(ordered_ids_);
    }

    constexpr const id_type& operator[](std::size_t idx) const noexcept
    {
        assert(idx < size_);
        return ordered_ids_[idx];
    }

    // compare to a single id, only works if the ordered_ids only holds a single
    // typed_id, otherwise returns false
    constexpr bool operator==(const id_type& id) const noexcept
    {
        if constexpr (sizeof...(Ts) == 1)
        {
            return contains(id);
        }
        else
        {
            return false;
        }
    }

    constexpr bool operator!=(const id_type& id) const noexcept
    {
        return (*this == id);
    }

    // compare with a typed_id, returns true if the type is equal to the type in
    // ordered_ids. Otherwise returns false
    template<typename U>
    constexpr bool operator==(const matter::typed_id<id_type, U>&) const
        noexcept
    {
        if constexpr (sizeof...(Ts) == 1)
        {
            return detail::type_in_list_v<U, Ts...>;
        }
        else
        {
            return false;
        }
    }

    template<typename U>
    constexpr bool operator!=(const matter::typed_id<id_type, U>& tid) const
        noexcept
    {
        return !(*this == tid);
    }

    constexpr bool operator==(const ordered_typed_ids&) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const ordered_typed_ids& other) const noexcept
    {
        return !(*this == other);
    }

    // compare to another ordered_ids object, if the sizes are unequal return
    // false. if all types are in both, return true. Otherwise false
    template<typename... Us>
    constexpr bool
    operator==(const matter::ordered_typed_ids<id_type, Us...>&) const noexcept
    {
        if constexpr (sizeof...(Ts) != sizeof...(Us))
        {
            return false;
        }
        else if constexpr ((detail::type_in_list_v<Ts, Us...> && ...))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template<typename... Us>
    constexpr bool
    operator!=(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        return !(*this == other);
    }

    template<typename... Us>
    constexpr std::enable_if_t<sizeof...(Ts) == sizeof...(Us), bool>
    operator<(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        for (std::size_t i = 0; i < size(); ++i)
        {
            if (ordered_ids_[i] < other.ordered_ids_[i])
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Us>
    constexpr std::enable_if_t<sizeof...(Ts) == sizeof...(Us), bool>
    operator>(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        for (std::size_t i = 0; i < size(); ++i)
        {
            if (ordered_ids_[i] > other.ordered_ids_[i])
            {
                return true;
            }
        }

        return false;
    }

    // forward this operation to the equals operator, should return true because
    // all same types are contained
    constexpr bool contains(const ordered_typed_ids& other) const noexcept
    {
        return *this == other;
    }

    template<typename... Us>
    constexpr bool
    contains(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        constexpr auto other_size =
            matter::ordered_typed_ids<id_type, Us...>::size_;
        static_assert(size_ >= other_size);

        return std::includes(begin(), end(), other.begin(), other.end());
    }

    constexpr bool contains(const id_type& id) const noexcept
    {
        return std::binary_search(begin(), end(), id);
    }

    constexpr const std::array<id_type, sizeof...(Ts)> array() const noexcept
    {
        return ordered_ids_;
    }
};

template<typename Id, typename... Ts>
ordered_typed_ids(const matter::typed_id<Id, Ts>&... tids) noexcept
    ->ordered_typed_ids<Id, Ts...>;

template<typename Id, typename... Ts>
ordered_typed_ids(const matter::unordered_typed_ids<Id, Ts...>& ids) noexcept
    ->ordered_typed_ids<Id, Ts...>;
} // namespace matter

namespace std
{
template<typename Id, typename... Ts>
struct tuple_size<matter::unordered_typed_ids<Id, Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t N, typename Id, typename... Ts>
struct tuple_element<N, matter::unordered_typed_ids<Id, Ts...>>
{
    using type = decltype(std::declval<matter::unordered_typed_ids<Id, Ts...>>()
                              .template get<N>());
};

template<typename Id, typename... Ts>
struct tuple_size<matter::ordered_typed_ids<Id, Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t N, typename Id, typename... Ts>
struct tuple_element<N, matter::ordered_typed_ids<Id, Ts...>>
{
    using type = decltype(
        std::declval<matter::ordered_typed_ids<Id, Ts...>>().template get<N>());
};
} // namespace std

#endif
