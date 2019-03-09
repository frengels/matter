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
        return id_ == other.id_;
    }

    constexpr bool operator!=(const id_erased& other) const noexcept
    {
        return id_ != other.id_;
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
                                     const id_erased& erased) noexcept;
    friend constexpr bool operator!=(id_type          id,
                                     const id_erased& erased) noexcept;
    friend constexpr bool operator<(id_type          id,
                                    const id_erased& erased) noexcept;
    friend constexpr bool operator>(id_type          id,
                                    const id_erased& erased) noexcept;
    friend constexpr bool operator<=(id_type          id,
                                     const id_erased& erased) noexcept;
    friend constexpr bool operator>=(id_type          id,
                                     const id_erased& erased) noexcept;
};
constexpr bool operator==(id_erased::id_type id,
                          const id_erased&   erased) noexcept
{
    return id == erased.id_;
}

constexpr bool operator!=(id_erased::id_type id,
                          const id_erased&   erased) noexcept
{
    return id != erased.id_;
}

constexpr bool operator<(id_erased::id_type id,
                         const id_erased&   erased) noexcept
{
    return id < erased.id_;
}

constexpr bool operator>(id_erased::id_type id,
                         const id_erased&   erased) noexcept
{
    return id > erased.id_;
}

constexpr bool operator<=(id_erased::id_type id,
                          const id_erased&   erased) noexcept
{
    return id <= erased.id_;
}

constexpr bool operator>=(id_erased::id_type id,
                          const id_erased&   erased) noexcept
{
    return id >= erased.id_;
}
} // namespace matter

#endif