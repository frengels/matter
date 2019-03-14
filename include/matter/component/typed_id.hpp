#ifndef MATTER_COMPONENT_TYPED_ID_HPP
#define MATTER_COMPONENT_TYPED_ID_HPP

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include <utility>

#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Cs>
class component_identifier;

template<typename Id, typename... Ts>
class unordered_typed_ids;

template<typename Id, typename C, bool S = false>
struct typed_id
{
    template<typename... Cs>
    friend class component_identifier;

    template<typename Id2, typename... Ts>
    friend class unordered_typed_ids;

    static constexpr auto _is_static = S;

    using id_type = Id;
    using type    = C;

private:
    const id_type id_;

private:
    explicit constexpr typed_id(id_type id) : id_{id}
    {}

public:
    constexpr typed_id(const typed_id& other) noexcept = default;
    constexpr typed_id& operator=(const typed_id& other) noexcept = default;

    static constexpr bool is_static() noexcept
    {
        return _is_static;
    }

    constexpr bool operator==(const typed_id& other) const noexcept
    {
        return true;
    }

    constexpr bool operator!=(const typed_id& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename UId, typename U, bool CX>
    constexpr bool operator==(const typed_id<UId, U, CX>& other) const noexcept
    {
        return false;
    }

    template<typename UId, typename U, bool CX>
    constexpr bool operator!=(const typed_id<UId, U, CX>& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename U>
    constexpr std::enable_if_t<std::is_convertible_v<U, id_type>, bool>
    operator==(U&& id) const noexcept
    {
        return get() == static_cast<id_type>(std::forward<U>(id));
    }

    template<typename U>
    friend constexpr std::enable_if_t<std::is_convertible_v<U, id_type>, bool>
    operator==(U&& lhs, const typed_id& rhs) noexcept
    {
        return static_cast<id_type>(std::forward<U>(lhs)) == rhs.get();
    }

    template<typename U>
    constexpr std::enable_if_t<std::is_convertible_v<U, id_type>, bool>
    operator!=(U&& id) const noexcept
    {
        return !(*this == std::forward<U>(id));
    }

    template<typename U>
    friend constexpr std::enable_if_t<std::is_convertible_v<U, id_type>, bool>
    operator!=(U&& lhs, const typed_id& rhs) noexcept
    {
        return !(std::forward<U>(lhs) == rhs);
    }

    constexpr id_type get() const noexcept
    {
        return id_;
    }
    constexpr operator id_type() const noexcept
    {
        return get();
    }
};

template<typename T>
struct is_typed_id : std::false_type
{};

template<typename Id, typename C, bool S>
struct is_typed_id<matter::typed_id<Id, C, S>> : std::true_type
{};

template<typename T>
static constexpr auto is_typed_id_v = is_typed_id<T>::value;

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

    static constexpr auto _size = sizeof...(Ts);

public:
    using id_type = Id;

private:
    const std::array<id_type, _size> ids_;

public:
    constexpr unordered_typed_ids(Ts... tids) noexcept : ids_{tids.get()...}
    {}

    static constexpr bool is_static() noexcept
    {
        return (Ts::is_static() && ...);
    }

    constexpr auto size() const noexcept
    {
        return _size;
    }

    template<typename T>
    constexpr T get() const noexcept
    {
        static_assert(is_typed_id_v<T>, "T must be a full typed_id");
        static_assert(detail::type_index<T, Ts...>().has_value());
        constexpr auto idx = detail::type_index<T, Ts...>().value();
        return typed_id<id_type, typename T::type, T::is_static()>{
            std::get<idx>(ids_)};
    }

    template<std::size_t I>
    constexpr auto get() const noexcept -> decltype(std::get<I>(ids_))
    {
        return std::get<I>(ids_);
    }
};

template<typename... Ts>
unordered_typed_ids(Ts... tids) noexcept
    ->unordered_typed_ids<typename detail::nth_t<0, Ts...>::id_type, Ts...>;

template<typename Id, typename... Ts>
class ordered_typed_ids {
    static_assert(sizeof...(Ts) >= 1, "Must hold at least one typed_id.");
    static_assert((is_typed_id_v<Ts> && ...),
                  "All Ts... must be of type 'typed_id'.");
    static_assert((std::is_same_v<Id, typename Ts::id_type> && ...),
                  "All id_type must be of the same type.");

public:
    using id_type = Id;

    static constexpr auto _size = sizeof...(Ts);

private:
    const std::array<Id, _size> ordered_ids_;

public:
    constexpr ordered_typed_ids(Ts... types) noexcept
        : ordered_ids_{[&]() -> std::array<id_type, _size> {
              // no sorting required
              if constexpr (size() == 1)
              {
                  return {types...};
              }
              else if constexpr ((types.is_constexpr() && ...))
              {
                  // guarantee evaluation at compile time
                  constexpr std::array arr{types...};
                  std::sort(arr.begin(), arr.end());
                  return arr;
              }
              else
              {
                  // runtime sorting for runtime typed_ids
                  std::array arr{types...};
                  std::sort(arr.begin(), arr.end());
                  return arr;
              }
          }()}
    {}

    constexpr ordered_typed_ids(
        const unordered_typed_ids<id_type, Ts...>& ids) noexcept
        : ordered_ids_{[&]() -> std::array<id_type, _size> {
              if constexpr (size() == 1)
              {
                  // single element case doesn't require sorting
                  return {ids.template get<0>()};
              }
              else if constexpr (ids.is_static())
              {
                  // guarantee consteval
                  // TODO: can't capture ids as constexpr for now
                  auto arr = ids.ids_;
                  std::sort(arr.begin(), arr.end());
                  return arr;
              }
              else
              {
                  // peasant runtime variables
                  auto arr = ids.ids_;
                  std::sort(arr.begin(), arr.end());
                  return arr;
              }
          }()}
    {}

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
        return _size;
    }

    static constexpr auto is_static() noexcept
    {
        return (Ts::is_static() && ...);
    }

    constexpr auto operator[](std::size_t idx) const noexcept
    {
        assert(idx < _size);
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
        static_assert(size() == other.size());
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
        static_assert(size() == other.size());

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
        static_assert(size() == other.size());

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
        static_assert(size() >= other.size());

        if constexpr (other.size() == 1)
        {
            return contains(other[0]);
        }
        else
        {
            return std::includes(begin(), end(), other.begin(), other.end());
        }
    }

    constexpr bool contains(id_type id) const noexcept
    {
        auto it = std::lower_bound(begin(), end(), id);
        return it == end() ? false : true;
    }
};

template<typename... Ts>
ordered_typed_ids(Ts... types) noexcept
    ->ordered_typed_ids<typename detail::nth<0, Ts...>::id_type, Ts...>;
} // namespace matter

#endif
