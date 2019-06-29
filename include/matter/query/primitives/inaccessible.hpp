#ifndef MATTER_QUERY_PRIMITIVES_INACCESSIBLE_HPP
#define MATTER_QUERY_PRIMITIVES_INACCESSIBLE_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/query/runtime.hpp"
#include "matter/util/empty.hpp"

namespace matter
{
namespace detail
{
template<typename T>
class inaccessible_impl {
public:
    using element_type = T;

private:
    bool valid_{false};

public:
    constexpr inaccessible_impl() noexcept = default;
    constexpr inaccessible_impl(std::nullptr_t) noexcept : valid_{false}
    {}

    constexpr inaccessible_impl(T* ptr) noexcept : valid_{ptr}
    {}

    constexpr inaccessible_impl<T>& operator=(T* ptr) noexcept
    {
        valid_ = ptr;
        return *this;
    }

    explicit constexpr operator bool() const noexcept
    {
        return valid_;
    }

    constexpr matter::empty operator*() const noexcept
    {
        return {};
    }
};
} // namespace detail

namespace prim
{
struct inaccessible
{
    struct storage_modifier
    {
        template<typename T>
        constexpr matter::detail::inaccessible_impl<T>
        operator()(T* store) const noexcept
        {
            return matter::detail::inaccessible_impl{store};
        }
    };

    static constexpr matter::access access_enum() noexcept
    {
        return matter::access::inaccessible;
    }
};
} // namespace prim
} // namespace matter

#endif
