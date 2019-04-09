#ifndef MATTER_UTIL_ID_ERASED_HPP
#define MATTER_UTIL_ID_ERASED_HPP

#pragma once

#include "matter/util/erased.hpp"

namespace matter
{
class id_erased final {
public:
    using id_type = std::size_t;

private:
    id_type        id_;
    matter::erased erased_;

public:
    template<typename T, typename... Args>
    id_erased(id_type id, std::in_place_type_t<T>, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<erased,
                                        std::in_place_type_t<T>,
                                        Args&&...>)
        : id_{id}, erased_{std::in_place_type_t<T>{},
                           std::forward<Args>(args)...}
    {}

    id_erased(id_erased&& other) noexcept
        : id_{std::move(other.id_)}, erased_{std::move(other.erased_)}
    {}

    constexpr erased& base() noexcept
    {
        return erased_;
    }

    constexpr const erased& base() const noexcept
    {
        return erased_;
    }

    id_erased& operator=(id_erased&& other) noexcept
    {
        id_     = std::move(other.id_);
        erased_ = std::move(other.erased_);

        return *this;
    }

    constexpr id_type id() const noexcept
    {
        return id_;
    }

    bool empty() const noexcept
    {
        return erased_.empty();
    }

    void clear() noexcept
    {
        erased_.clear();
    }

    constexpr void* get_void() noexcept
    {
        return erased_.get_void();
    }

    constexpr const void* get_void() const noexcept
    {
        return erased_.get_void();
    }

    template<typename T>
    constexpr T& get() noexcept
    {
        return erased_.get<T>();
    }

    template<typename T>
    constexpr const T& get() const noexcept
    {
        return erased_.get<T>();
    }

    constexpr bool operator==(const id_erased& other) const noexcept
    {
        return id_ == other.id_ && get_void() == other.get_void();
    }

    constexpr bool operator!=(const id_erased& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const id_erased& other) const noexcept
    {
        return id_ < other.id_;
    }

    constexpr bool operator>(const id_erased& other) const noexcept
    {
        return id_ > other.id_;
    }

    constexpr bool operator<=(const id_erased& other) const noexcept
    {
        return id_ <= other.id_;
    }

    constexpr bool operator>=(const id_erased& other) const noexcept
    {
        return id_ >= other.id_;
    }

    constexpr bool operator==(id_type id) const noexcept
    {
        return id_ == id;
    }

    constexpr bool operator!=(id_type id) const noexcept
    {
        return id_ != id;
    }

    constexpr bool operator<(id_type id) const noexcept
    {
        return id_ < id;
    }

    constexpr bool operator>(id_type id) const noexcept
    {
        return id_ > id;
    }

    constexpr bool operator<=(id_type id) const noexcept
    {
        return id_ <= id;
    }

    constexpr bool operator>=(id_type id) const noexcept
    {
        return id_ >= id;
    }

    friend constexpr bool operator==(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id == erased.id_;
    }

    friend constexpr bool operator!=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id != erased.id_;
    }

    friend constexpr bool operator<(id_type          id,
                                    const id_erased& erased) noexcept
    {
        return id < erased.id_;
    }

    friend constexpr bool operator>(id_type          id,
                                    const id_erased& erased) noexcept
    {
        return id > erased.id_;
    }

    friend constexpr bool operator<=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id <= erased.id_;
    }

    friend constexpr bool operator>=(id_type          id,
                                     const id_erased& erased) noexcept
    {
        return id >= erased.id_;
    }

    friend void swap(id_erased& lhs, id_erased& rhs) noexcept
    {
        using std::swap;
        swap(lhs.id_, rhs.id_);
        swap(lhs.erased_, rhs.erased_);
    }
};
} // namespace matter

#endif
