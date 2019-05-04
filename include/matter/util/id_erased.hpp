#ifndef MATTER_UTIL_ID_ERASED_HPP
#define MATTER_UTIL_ID_ERASED_HPP

#pragma once

#include <numeric>

#include "matter/component/id.hpp"
#include "matter/util/erased.hpp"

namespace matter
{
template<typename Id>
class id_erased : public matter::erased {
    static_assert(matter::is_id_v<Id>);

public:
    using id_type   = Id;
    using base_type = matter::erased;

private:
    id_type id_{};

public:
    constexpr id_erased() noexcept = default;

    template<typename T, typename... Args>
    id_erased(const id_type& id,
              std::in_place_type_t<T>,
              Args&&... args) noexcept(std::
                                           is_nothrow_constructible_v<
                                               erased,
                                               std::in_place_type_t<T>,
                                               Args&&...>)
        : base_type{std::in_place_type_t<T>{}, std::forward<Args>(args)...},
          id_{id}
    {}

    id_erased(id_erased&& other) noexcept
        : base_type{std::move(other)}, id_{std::move(other.id_)}
    {}

    constexpr base_type& base() noexcept
    {
        return *this;
    }

    constexpr const base_type& base() const noexcept
    {
        return *this;
    }

    id_erased& operator=(id_erased&& other) noexcept
    {
        base_type::operator=(std::move(other));
        id_                = std::move(other.id_);

        return *this;
    }

    constexpr id_type id() const noexcept
    {
        return id_;
    }

    void clear() noexcept
    {

        id_ = std::numeric_limits<id_type>::max();
        base_type::clear();
    }

    constexpr bool operator==(const id_erased& other) const noexcept
    {
        return id() == other.id() && base_type::operator==(other);
    }

    constexpr bool operator!=(const id_erased& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const id_erased& other) const noexcept
    {
        return id() < other.id();
    }

    constexpr bool operator>(const id_erased& other) const noexcept
    {
        return id() > other.id();
    }

    constexpr bool operator<=(const id_erased& other) const noexcept
    {
        return id() <= other.id();
    }

    constexpr bool operator>=(const id_erased& other) const noexcept
    {
        return id() >= other.id();
    }

    constexpr bool operator==(id_type other_id) const noexcept
    {
        return id() == other_id;
    }

    constexpr bool operator!=(id_type other_id) const noexcept
    {
        return !(*this == other_id);
    }

    constexpr bool operator<(id_type other_id) const noexcept
    {
        return id() < other_id;
    }

    constexpr bool operator>(id_type other_id) const noexcept
    {
        return id() > other_id;
    }

    constexpr bool operator<=(id_type other_id) const noexcept
    {
        return id() <= other_id;
    }

    constexpr bool operator>=(id_type other_id) const noexcept
    {
        return id_ >= other_id;
    }

    friend constexpr bool operator==(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id == erased.id();
    }

    friend constexpr bool operator!=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id != erased.id();
    }

    friend constexpr bool operator<(id_type          id,
                                    const id_erased& erased) noexcept
    {
        return id < erased.id();
    }

    friend constexpr bool operator>(id_type          id,
                                    const id_erased& erased) noexcept
    {
        return id > erased.id();
    }

    friend constexpr bool operator<=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id <= erased.id();
    }

    friend constexpr bool operator>=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id >= erased.id();
    }

    friend void swap(id_erased& lhs, id_erased& rhs) noexcept
    {
        using std::swap;
        swap(static_cast<erased&>(lhs), static_cast<erased&>(rhs));
        swap(lhs.id_, rhs.id_);
    }
};
} // namespace matter

#endif
