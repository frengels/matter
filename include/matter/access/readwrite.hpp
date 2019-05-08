#ifndef MATTER_ACCESS_READWRITE_HPP
#define MATTER_ACCESS_READWRITE_HPP

#pragma once

#include <optional>

#include "matter/access/entity_handle.hpp"
#include "matter/access/storage_handle.hpp"
#include "matter/component/any_group.hpp"

namespace matter
{
template<typename Registry, typename C>
class readwrite_meta;

template<typename C>
class readwrite {
public:
    template<typename Registry>
    using meta_type = readwrite_meta<Registry, C>;

private:
    const C&         val_;
    std::optional<C> write_to_;

public:
    constexpr readwrite(const C& val) noexcept : val_{val}
    {}

    constexpr const C& get() const noexcept
    {
        return val_;
    }

    template<typename... Args>
    constexpr C&
    stage(Args&&... args) noexcept(std::is_nothrow_constructible_v<C, Args...>)
    {
        write_to_ = C(std::forward<Args>(args)...);
        return *write_to_;
    }
};

template<typename Registry, typename C>
class readwrite_meta {
public:
    using required_types = C;

    using registry_type = Registry;
    using id_type       = typename registry_type::id_type;

public:
    constexpr readwrite_meta(registry_type&) noexcept
    {}

    constexpr readwrite<C>
    make_access(matter::entity_handle<id_type>     ent,
                matter::storage_handle<id_type, C> store) const noexcept
    {
        return readwrite<C>{store[ent]};
    }
};
} // namespace matter

#endif
