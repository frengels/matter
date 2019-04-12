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

#include "matter/util/algorithm.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Cs>
class component_identifier;

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

template<typename T, typename = void>
struct is_typed_id : std::false_type
{};

template<typename TId>
struct is_typed_id<
    TId,
    std::enable_if_t<
        std::is_default_constructible_v<TId> &&
            std::is_nothrow_copy_constructible_v<TId> &&
            std::is_nothrow_copy_assignable_v<TId> &&
            std::is_same_v<typename TId::id_type,
                           std::remove_cv_t<std::remove_reference_t<decltype(
                               std::declval<const TId>().value())>>> &&
            std::is_same_v<
                typename TId::id_type,
                std::remove_cv_t<std::remove_reference_t<decltype(
                    typename TId::id_type(std::declval<const TId&>()))>>> &&
            std::is_same_v<bool,
                           decltype(
                               std::declval<const typename TId::id_type&>() ==
                               std::declval<const typename TId::id_type&>())> &&
            std::is_same_v<bool,
                           decltype(
                               std::declval<const typename TId::id_type&>() !=
                               std::declval<const typename TId::id_type&>())> &&
            std::is_same_v<bool,
                           decltype(
                               std::declval<const typename TId::id_type&>() <
                               std::declval<const typename TId::id_type&>())> &&
            std::is_same_v<bool,
                           decltype(
                               std::declval<const typename TId::id_type&>() >
                               std::declval<const typename TId::id_type&>())> &&
            std::is_same_v<bool,
                           decltype(std::declval<const TId&>().has_value())>,
        std::void_t<typename TId::id_type, typename TId::type>>>
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

template<typename Id, typename C, Id Val>
struct static_id
{
    using id_type = Id;
    using type    = C;

private:
    static constexpr auto value_ = Val;

public:
    constexpr static_id() noexcept = default;

    constexpr auto value() const noexcept
    {
        return value_;
    }

    constexpr operator id_type() const noexcept
    {
        return value();
    }

    constexpr auto has_value() const noexcept
    {
        return true;
    }
};

template<typename Id, typename C>
struct runtime_id
{
    using id_type = Id;
    using type    = C;

private:
    id_type value_;

public:
    /// \brief default initialize into an invalid state
    constexpr runtime_id() noexcept
        : value_{std::numeric_limits<id_type>::max()}
    {}

    constexpr runtime_id(id_type val) noexcept : value_{val}
    {}

    constexpr auto value() const noexcept
    {
        return value_;
    }

    constexpr operator id_type() const noexcept
    {
        return value();
    }

    constexpr auto has_value() const noexcept
    {
        return value_ != std::numeric_limits<id_type>::max();
    }
};

template<typename Id, typename... Ts>
class ordered_typed_ids;

template<typename Id, typename... Ts>
class unordered_typed_ids {
    static_assert(sizeof...(Ts) >= 1, "Must hold at least one typed_id.");
    static_assert((is_typed_id_v<Ts> && ...),
                  "All Ts... must be of type 'typed_id'.");
    static_assert((std::is_same_v<Id, typename Ts::id_type> && ...),
                  "All id_type must be of the same type.");

    friend class ordered_typed_ids<Id, Ts...>;

    static constexpr auto size_ = sizeof...(Ts);

public:
    using id_type = Id;

private:
    std::tuple<Ts...> ids_;

public:
    constexpr unordered_typed_ids() noexcept : ids_{}
    {}

    constexpr unordered_typed_ids(Ts... tids) noexcept : ids_{tids...}
    {}

    constexpr auto size() const noexcept
    {
        return size_;
    }

    constexpr auto has_value() const noexcept
    {
        return (get<Ts>().has_value() && ...);
    }

    template<typename T>
    constexpr std::enable_if_t<!matter::is_typed_id_v<T>,
                               matter::common_typed_id_type_t<T, Ts...>>
    get() const noexcept
    {
        return std::get<matter::common_typed_id_type_t<T, Ts...>>(ids_);
    }

    template<typename TId>
    constexpr std::enable_if_t<matter::is_typed_id_v<TId>, TId> get() const
        noexcept
    {
        auto tid = std::get<TId>(ids_);
        assert(tid.has_value());
        return tid;
    }

    template<std::size_t I>
    constexpr auto get() const noexcept
    {
        auto tid = std::get<I>(ids_);
        assert(tid.has_value());
        return tid;
    }

    constexpr auto as_array() const noexcept
    {
        assert(has_value());
        return std::apply(
            [](auto... ids) { return std::array{ids.value()...}; }, ids_);
    }
};

template<typename... Ts>
unordered_typed_ids(Ts... tids) noexcept
    ->unordered_typed_ids<typename detail::nth_t<0, Ts...>::id_type, Ts...>;

template<typename T>
struct is_unordered_typed_ids : std::false_type
{};

template<typename Id, typename... TIds>
struct is_unordered_typed_ids<matter::unordered_typed_ids<Id, TIds...>>
    : std::true_type
{};

template<typename T>
static constexpr auto is_unordered_typed_ids_v =
    is_unordered_typed_ids<T>::value;

template<typename Id, typename... Ts>
class ordered_typed_ids {
    static_assert(sizeof...(Ts) >= 1, "Must hold at least one typed_id.");
    static_assert((is_typed_id_v<Ts> && ...),
                  "All Ts... must be of type 'typed_id'.");
    static_assert((std::is_same_v<Id, typename Ts::id_type> && ...),
                  "All id_type must be of the same type.");

public:
    using id_type = Id;

    static constexpr auto size_ = sizeof...(Ts);

private:
    std::array<Id, size_> ordered_ids_;

public:
    /// \brief default initialize ordered ids into an invalid state
    constexpr ordered_typed_ids() noexcept
        : ordered_ids_{[] {
              std::array<Id, size_> ids;
              for (auto& id : ids)
              {
                  id = std::numeric_limits<id_type>::max();
              }

              return ids;
          }()}
    {}

    constexpr ordered_typed_ids(Ts... types) noexcept
        : ordered_ids_{[&]() -> std::array<id_type, size_> {
              // no sorting required
              if constexpr (size_ == 1)
              {
                  return {types...};
              }
              else
              {
                  // runtime sorting for runtime typed_ids
                  std::array arr{types.value()...};
                  matter::static_sort(arr);
                  return arr;
              }
          }()}
    {}

    constexpr ordered_typed_ids(
        const unordered_typed_ids<id_type, Ts...>& ids) noexcept
        : ordered_ids_{[&]() -> std::array<id_type, size_> {
              if constexpr (size_ == 1)
              {
                  // single element case doesn't require sorting
                  return {ids.template get<0>()};
              }
              else
              { // peasant runtime variables
                  auto arr = ids.as_array();
                  matter::static_sort(arr);
                  return arr;
              }
          }()}
    {}

    constexpr auto has_value() const noexcept
    {
        if constexpr (sizeof...(Ts) == 0)
        {
            // an empty ordered_ids is always valid because it has no content
            return true;
        }
        else
        {
            return ordered_ids_[0] == std::numeric_limits<id_type>::max();
        }
    }

    template<std::size_t N>
    constexpr id_type get() const noexcept
    {
        return std::get<N>(ordered_ids_);
    }

    constexpr auto begin() const noexcept
    {
        return ordered_ids_.begin();
    }

    constexpr auto end() const noexcept
    {
        return ordered_ids_.end();
    }

    static constexpr auto size() noexcept
    {
        return size_;
    }

    constexpr auto operator[](std::size_t idx) const noexcept
    {
        assert(idx < size_);
        return ordered_ids_[idx];
    }

    constexpr bool operator==(const ordered_typed_ids&) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const ordered_typed_ids&) const noexcept
    {
        return false;
    }

    template<typename... Us>
    constexpr bool
    operator==(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        constexpr auto other_size =
            matter::ordered_typed_ids<id_type, Us...>::size_;
        static_assert(size_ == other_size);
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    template<typename... Us>
    constexpr bool
    operator!=(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        return !(*this == other);
    }

    template<typename... Us>
    constexpr bool
    operator<(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        constexpr auto other_size =
            matter::ordered_typed_ids<id_type, Us...>::size_;
        static_assert(size_ == other_size);

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
    constexpr bool
    operator>(const matter::ordered_typed_ids<id_type, Us...>& other) const
        noexcept
    {
        constexpr auto other_size =
            matter::ordered_typed_ids<id_type, Us...>::size_;
        static_assert(size_ == other_size);

        for (std::size_t i = 0; i < size(); ++i)
        {
            if (ordered_ids_[i] > other.ordered_ids_[i])
            {
                return true;
            }
        }

        return false;
    }

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

        if constexpr (other_size == 1)
        {
            return contains(other[0]);
        }
        else
        {
            return std::includes(begin(), end(), other.begin(), other.end());
        }
    }

    constexpr bool contains(const id_type& id) const noexcept
    {
        auto it = std::lower_bound(begin(), end(), id);

        if (it == end() || *it != id)
        {
            return false;
        }

        return true;
    }
};

template<typename... Ts>
ordered_typed_ids(Ts... types) noexcept
    ->ordered_typed_ids<typename detail::first_t<Ts...>::id_type, Ts...>;

template<typename Id, typename... Ts>
ordered_typed_ids(const unordered_typed_ids<Id, Ts...>& ids) noexcept
    ->ordered_typed_ids<Id, Ts...>;

template<typename T>
struct is_ordered_typed_ids : std::false_type
{};

template<typename Id, typename... TIds>
struct is_ordered_typed_ids<matter::ordered_typed_ids<Id, TIds...>>
    : std::true_type
{};

template<typename T>
static constexpr auto is_ordered_typed_ids_v = is_ordered_typed_ids<T>::value;
} // namespace matter

namespace std
{
template<typename Id, typename... TIds>
struct tuple_size<matter::unordered_typed_ids<Id, TIds...>>
    : std::integral_constant<std::size_t, sizeof...(TIds)>
{};

template<std::size_t N, typename Id, typename... TIds>
struct tuple_element<N, matter::unordered_typed_ids<Id, TIds...>>
{
    using type =
        decltype(std::declval<matter::unordered_typed_ids<Id, TIds...>>()
                     .template get<N>());
};

template<typename Id, typename... TIds>
struct tuple_size<matter::ordered_typed_ids<Id, TIds...>>
    : std::integral_constant<std::size_t, sizeof...(TIds)>
{};

template<std::size_t N, typename Id, typename... TIds>
struct tuple_element<N, matter::ordered_typed_ids<Id, TIds...>>
{
    using type = decltype(std::declval<matter::ordered_typed_ids<Id, TIds...>>()
                              .template get<N>());
};
} // namespace std

#endif
