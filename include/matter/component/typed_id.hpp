#ifndef MATTER_COMPONENT_TYPED_ID_HPP
#define MATTER_COMPONENT_TYPED_ID_HPP

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Cs>
class component_identifier;

template<typename Id, typename... Ts>
class unordered_typed_ids;

namespace detail
{
template<typename Id, Id Val>
struct typed_id_empty
{
protected:
    using id_type = Id;

protected:
    constexpr typed_id_empty() = default;
    constexpr typed_id_empty(id_type id)
    {
        (void) id;
    }

public:
    static constexpr auto is_static() noexcept
    {
        return true;
    }

    static constexpr auto value() noexcept
    {
        return Val;
    }
};

template<typename Id>
struct typed_id_base
{
protected:
    using id_type = Id;

    id_type id_;

protected:
    constexpr typed_id_base(id_type id) : id_{id}
    {}

public:
    static constexpr auto is_static() noexcept
    {
        return false;
    }

    constexpr auto value() const noexcept
    {
        return id_;
    }
};
} // namespace detail

template<typename Id, typename C, Id Val>
struct typed_id
    : public std::conditional_t<Val == std::numeric_limits<Id>::max(),
                                detail::typed_id_base<Id>,
                                detail::typed_id_empty<Id, Val>>
{
    template<typename... Cs>
    friend class component_identifier;

    template<typename Id2, typename... Ts>
    friend class unordered_typed_ids;

    static constexpr auto _value = Val;

    using _base_type = std::conditional_t<Val == std::numeric_limits<Id>::max(),
                                          detail::typed_id_base<Id>,
                                          detail::typed_id_empty<Id, Val>>;

    static constexpr auto _is_static = Val != std::numeric_limits<Id>::max();

    using id_type = typename _base_type::id_type;
    using type    = C;

private:
    using _base_type::_base_type;

public:
    constexpr typed_id(const typed_id& other) noexcept = default;
    constexpr typed_id& operator=(const typed_id& other) noexcept = default;

    constexpr bool operator==(const typed_id& other) const noexcept
    {
        (void) other;
        return true;
    }

    constexpr bool operator!=(const typed_id& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename UId, typename U, UId V>
    constexpr bool operator==(const typed_id<UId, U, V>& other) const noexcept
    {
        (void) other;
        return false;
    }

    template<typename UId, typename U, UId V>
    constexpr bool operator!=(const typed_id<UId, U, V>& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename U>
    constexpr std::enable_if_t<
        std::is_convertible_v<U, id_type> &&
            !std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>,
                            typed_id<id_type, type, _value>>,
        bool>
    operator==(U&& id) const noexcept
    {
        return this->value() == static_cast<id_type>(std::forward<U>(id));
    }

    template<typename U>
    friend constexpr std::enable_if_t<
        std::is_convertible_v<U, id_type> &&
            !std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>,
                            typed_id<id_type, type, _value>>,
        bool>
    operator==(U&& lhs, const typed_id& rhs) noexcept
    {
        return static_cast<id_type>(std::forward<U>(lhs)) == rhs.value();
    }

    template<typename U>
    constexpr std::enable_if_t<
        std::is_convertible_v<U, id_type> &&
            !std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>,
                            typed_id<id_type, type, _value>>,
        bool>
    operator!=(U&& id) const noexcept
    {
        return !(*this == std::forward<U>(id));
    }

    template<typename U>
    friend constexpr std::enable_if_t<
        std::is_convertible_v<U, id_type> &&
            !std::is_same_v<std::remove_cv_t<std::remove_reference_t<U>>,
                            typed_id<id_type, type, _value>>,
        bool>
    operator!=(U&& lhs, const typed_id& rhs) noexcept
    {
        return !(std::forward<U>(lhs) == rhs);
    }

    constexpr operator id_type() const noexcept
    {
        return this->value();
    }
};

template<typename T>
struct is_typed_id : std::false_type
{};

template<typename Id, typename C, Id V>
struct is_typed_id<matter::typed_id<Id, C, V>> : std::true_type
{};

template<typename T>
static constexpr auto is_typed_id_v = is_typed_id<T>::value;

template<typename Id, typename... Ts>
class ordered_typed_ids;

namespace detail
{
template<typename Id, bool S, typename... Ts>
struct unordered_typed_ids_base
{
    static_assert(!(Ts::is_static() && ...),
                  "All Ts... must be runtime typed_ids.");
    // assume all Ts... are already valid `typed_id`, verification happens in
    // the derived class

    using id_type = Id;

protected:
    const std::tuple<Ts...> ids_;

    constexpr unordered_typed_ids_base(Ts... tids) noexcept : ids_{tids...}
    {}

public:
    static constexpr auto is_static() noexcept
    {
        return false;
    }

protected:
    constexpr std::array<id_type, sizeof...(Ts)> as_array() const noexcept
    {
        return std::apply(
            [](auto... tids) -> std::array<id_type, sizeof...(Ts)> {
                return {tids.value()...};
            },
            ids_);
    }
};

template<typename Id, typename... Ts>
struct unordered_typed_ids_base<Id, true, Ts...>
{
    static_assert((Ts::is_static() && ...),
                  "All Ts... must be static typed_ids.");

    using id_type = Id;

protected:
    constexpr unordered_typed_ids_base(Ts...) noexcept
    {}

public:
    static constexpr auto is_static() noexcept
    {
        return true;
    }

protected:
    static constexpr std::array<id_type, sizeof...(Ts)> as_array() noexcept
    {
        return {Ts::value()...};
    }
};
} // namespace detail

template<typename Id, typename... Ts>
class unordered_typed_ids
    : public detail::
          unordered_typed_ids_base<Id, (Ts::is_static() && ...), Ts...> {
    static_assert(sizeof...(Ts) >= 1, "Must hold at least one typed_id.");
    static_assert((is_typed_id_v<Ts> && ...),
                  "All Ts... must be of type 'typed_id'.");
    static_assert((std::is_same_v<Id, typename Ts::id_type> && ...),
                  "All id_type must be of the same type.");

    friend class ordered_typed_ids<Id, Ts...>;

    static constexpr auto _size = sizeof...(Ts);

    using _base_type =
        detail::unordered_typed_ids_base<Id, (Ts::is_static() && ...), Ts...>;

public:
    using id_type = typename _base_type::id_type;

public:
    constexpr unordered_typed_ids(Ts... tids) noexcept : _base_type{tids...}
    {}

    static constexpr bool is_static() noexcept
    {
        return (Ts::is_static() && ...);
    }

    static constexpr auto size() noexcept
    {
        return _size;
    }

    template<typename T>
    constexpr T get() const noexcept
    {
        static_assert(is_typed_id_v<T>, "T must be a full typed_id");
        static_assert(detail::type_index<T, Ts...>().has_value(),
                      "T is not a held type");
        constexpr auto idx = detail::type_index<T, Ts...>().value();

        return get<idx>();
    }

    template<std::size_t I>
    constexpr auto get() const noexcept
    {
        static_assert(I < size(), "I is out of bounds.");
        using return_type = detail::nth_t<I, Ts...>;
        if constexpr (return_type::is_static())
        {
            return return_type{return_type::value()};
        }
        else
        {
            return return_type{std::get<I>(this->ids_)};
        }
    }
};

template<typename... Ts>
unordered_typed_ids(Ts... tids) noexcept
    ->unordered_typed_ids<typename detail::nth_t<0, Ts...>::id_type, Ts...>;

namespace detail
{
/// we can't directly sort constexpr objects, so use this function to sort it
/// instead.
template<typename T, std::size_t N>
constexpr std::array<T, N> sort_workaround(const std::array<T, N>& arr) noexcept
{
    auto sorted = arr;
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}
} // namespace detail

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
              else if constexpr ((Ts::is_static() && ...))
              {
                  // guarantee evaluation at compile time
                  constexpr std::array arr{Ts::value()...};
                  return detail::sort_workaround(arr);
              }
              else
              {
                  // runtime sorting for runtime typed_ids
                  std::array arr{types.value()...};
                  std::sort(arr.begin(), arr.end());
                  return arr;
              }
          }()}
    {}

    constexpr ordered_typed_ids(
        const unordered_typed_ids<id_type, Ts...>& ids) noexcept
        : ordered_ids_{[&]() -> std::array<id_type, _size> {
              using unordered_type = unordered_typed_ids<id_type, Ts...>;
              if constexpr (size() == 1)
              {
                  // single element case doesn't require sorting
                  return {ids.template get<0>()};
              }
              else if constexpr (unordered_type::is_static())
              {
                  // guarantee consteval
                  constexpr auto arr = unordered_type::as_array();
                  return detail::sort_workaround(arr);
              }
              else
              {
                  // peasant runtime variables
                  auto arr = ids.as_array();
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
        constexpr auto other_size =
            matter::ordered_typed_ids<id_type, Us...>::size();
        static_assert(size() == other_size);
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
            matter::ordered_typed_ids<id_type, Us...>::size();
        static_assert(size() == other_size);

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
            matter::ordered_typed_ids<id_type, Us...>::size();
        static_assert(size() == other_size);

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
            matter::ordered_typed_ids<id_type, Us...>::size();
        static_assert(size() >= other_size);

        if constexpr (other_size == 1)
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

template<typename Id, typename... Ts>
ordered_typed_ids(const unordered_typed_ids<Id, Ts...>& ids) noexcept
    ->ordered_typed_ids<Id, Ts...>;
} // namespace matter

#endif
