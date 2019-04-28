#ifndef MATTER_ACCESS_WRITE_HPP
#define MATTER_ACCESS_WRITE_HPP

#pragma once

#include <optional>

#include "matter/access/entity_handle.hpp"
#include "matter/access/storage_handle.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/component/group_vector.hpp"
#include "matter/component/registry.hpp"

namespace matter
{
template<typename Registry, typename C>
class write_meta;

template<typename C>
class write {
public:
    template<typename Registry>
    using meta_type = write_meta<Registry, C>;

private:
    std::optional<C> write_to_;

public:
    constexpr write() noexcept = default;

    template<typename... Args>
    constexpr C&
    stage(Args&&... args) noexcept(std::is_nothrow_constructible_v<C, Args...>)
    {
        write_to_ = C(std::forward<Args>(args)...);
        return *write_to_;
    }
};

template<typename Registry, typename C>
class write_meta {
public:
    using required_types = C;

    using registry_type = Registry;

public:
    constexpr write_meta(registry_type&) noexcept
    {}

    constexpr void process_group_vector(matter::group_vector&) const noexcept
    {}

    constexpr void process_group(const_any_group) const noexcept
    {}

    constexpr write<C> make_access(matter::entity_handle,
                                   matter::storage_handle<C>) const noexcept
    {
        return write<C>{};
    }
};

static_assert(matter::is_access_v<matter::write<int>, matter::registry<>>);
static_assert(matter::is_meta_access_v<write_meta<matter::registry<>, int>>);
} // namespace matter

#endif
