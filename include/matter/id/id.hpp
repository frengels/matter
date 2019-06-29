#ifndef MATTER_ID_ID_TYPE_HPP
#define MATTER_ID_ID_TYPE_HPP

#pragma once

#include <limits>
#include <type_traits>

#include "matter/util/concepts.hpp"

namespace matter
{
template<typename Id, typename = void>
struct is_id : std::false_type
{};

// defines the concept for an id. The idea behind the concept is that ids have
// to be strictly ordered and equality comparable (for sorting and allowing
// binary search). Default construction should be supported to initialize the id
// into a default and invalid state. the underlying id value can be retrieve
// using `Id::value()` and to confirm the current Id is valid `operator bool`
// should be provided. Using the default constructor and constructing from
// `Id::invalid_id` should yield equivalent results.
template<typename Id>
struct is_id<
    Id,
    std::enable_if_t<
        std::is_default_constructible_v<Id> &&
        std::is_nothrow_copy_constructible_v<Id> &&
        std::is_nothrow_copy_assignable_v<Id> && matter::is_swappable_v<Id> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() <
                                std::declval<const Id>())> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() >
                                std::declval<const Id>())> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() <=
                                std::declval<const Id>())> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() >=
                                std::declval<const Id>())> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() ==
                                std::declval<const Id>())> &&
        std::is_same_v<bool,
                       decltype(std::declval<const Id>() !=
                                std::declval<const Id>())> &&
        std::is_same_v<bool, decltype(bool(std::declval<const Id>()))>>>
    : std::true_type
{};

template<typename Id>
constexpr bool is_id_v = matter::is_id<Id>::value;

template<typename UnsignedIntegral>
class unsigned_id {
    static_assert(std::is_unsigned_v<UnsignedIntegral>,
                  "This class is specifically made for unsigned integrals");

public:
    using value_type = UnsignedIntegral;

    static constexpr value_type invalid_id =
        std::numeric_limits<value_type>::max();

private:
    // our id value, by default it's set to max
    value_type value_{invalid_id};

public:
    constexpr unsigned_id() noexcept = default;

    explicit constexpr unsigned_id(const value_type& id) noexcept : value_{id}
    {}

    constexpr value_type value() const noexcept
    {
        return value_;
    }

public:
    constexpr explicit operator bool() const noexcept
    {
        return value_ != invalid_id;
    }

    constexpr bool operator==(const unsigned_id& other) const noexcept
    {
        return value() == other.value();
    }

    constexpr bool operator!=(const unsigned_id& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const unsigned_id& other) const noexcept
    {
        return value() < other.value();
    }

    constexpr bool operator>(const unsigned_id& other) const noexcept
    {
        return value() > other.value();
    }

    constexpr bool operator<=(const unsigned_id& other) const noexcept
    {
        return value() <= other.value();
    }

    constexpr bool operator>=(const unsigned_id& other) const noexcept
    {
        return value() >= other.value();
    }

    constexpr unsigned_id& operator++() noexcept
    {
        ++value_;
        return *this;
    }

    constexpr unsigned_id& operator--() noexcept
    {
        --value_;
        return *this;
    }

    constexpr unsigned_id operator++(int) noexcept
    {
        auto r = *this;
        ++(*this);
        return r;
    }

    constexpr unsigned_id operator--(int) noexcept
    {
        auto r = *this;
        --(*this);
        return r;
    }

    friend void swap(unsigned_id& lhs, unsigned_id& rhs) noexcept
    {
        using std::swap;
        swap(lhs.value_, rhs.value_);
    }
};

static_assert(matter::is_id_v<unsigned_id<std::size_t>>);

template<typename SignedIntegral>
class signed_id {
    static_assert(std::is_signed_v<SignedIntegral>,
                  "This class is specifically made for signed integrals");

public:
    using value_type = SignedIntegral;

    static constexpr value_type invalid_id = -1;

private:
    value_type value_{invalid_id};

public:
    constexpr signed_id() noexcept = default;

    explicit constexpr signed_id(const value_type& id) noexcept : value_{id}
    {}

    constexpr value_type value() const noexcept
    {
        return value_;
    }

    explicit constexpr operator bool() const noexcept
    {
        return value_ != invalid_id;
    }

    constexpr bool operator==(const signed_id& other) const noexcept
    {
        return value() == other.value();
    }

    constexpr bool operator!=(const signed_id& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const signed_id& other) const noexcept
    {
        return value() < other.value();
    }

    constexpr bool operator>(const signed_id& other) const noexcept
    {
        return value() > other.value();
    }

    constexpr bool operator<=(const signed_id& other) const noexcept
    {
        return value() <= other.value();
    }

    constexpr bool operator>=(const signed_id& other) const noexcept
    {
        return value() >= other.value();
    }

    constexpr signed_id& operator++() noexcept
    {
        ++value_;
        return *this;
    }

    constexpr signed_id& operator--() noexcept
    {
        --value_;
        return *this;
    }

    constexpr signed_id operator++(int) noexcept
    {
        auto r = *this;
        ++(*this);
        return r;
    }

    constexpr signed_id operator--(int) noexcept
    {
        auto r = *this;
        --(*this);
        return r;
    }

    friend void swap(signed_id& lhs, signed_id& rhs) noexcept
    {
        using std::swap;
        swap(lhs.value_, rhs.value_);
    }
};

static_assert(matter::is_id_v<signed_id<int>>);
} // namespace matter

#endif
