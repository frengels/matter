#ifndef MATTER_UTIL_VARIANT_HPP
#define MATTER_UTIL_VARIANT_HPP

#include <algorithm>
#include <type_traits>

#include "matter/component/identifier.hpp"
#include "type_list.hpp"

#pragma once

namespace matter
{
template<std::size_t BufSize, typename... Ts>
struct variant
{
private:
    static constexpr std::size_t buffer_size_ = std::max({sizeof...(Ts)});

    static_assert(BufSize >= buffer_size_,
                  "Specified buffer size does not fit all listed types.");

public:
    // amount of bytes available for types to occupy
    static constexpr std::size_t buffer_size = BufSize;
    // type which generates new ids for additional variant types
    using identifier_type = matter::identifier<matter::variant<BufSize, Ts...>>;

private:
    std::size_t type_index_;
    char        buffer_[buffer_size];

public:
    template<typename T, typename... Args>
    constexpr variant(std::in_place_type_t<T>, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : type_index_{matter::detail::index_of<T, Ts...>::value}
    {
        static_assert(matter::detail::contains<T, Ts...>::value,
                      "Variant does not contain this type");
        new (buffer_) T(std::forward<Args>(args)...);
    }

    template<typename T>
    constexpr auto get() noexcept -> T&
    {
        // required to be constexpr
        if constexpr (std::remove_pointer_t<decltype(
                          this)>::template is_native_impl<T>())
        {
            auto idx = matter::detail::index_of<T, Ts...>::value;
            assert(idx == type_index_);
            return *reinterpret_cast<T*>(buffer_);
        }
        else
        {}
    }

    template<typename T>
    constexpr auto is_native() const noexcept -> bool
    {
        return std::remove_pointer_t<decltype(
            this)>::template is_native_impl<T>();
    }

private:
    template<typename T>
    static constexpr auto is_native_impl() noexcept -> bool
    {
        return matter::detail::contains<T, Ts...>::value;
    }
};
} // namespace matter

#endif
