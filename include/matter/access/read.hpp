#ifndef MATTER_ACCESS_READ_HPP
#define MATTER_ACCESS_READ_HPP

#pragma once

#include "matter/access/entity_handle.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/component/registry.hpp"

namespace matter
{
template<typename Registry, typename C>
struct read_meta;

template<typename C>
struct read
{
    template<typename Registry>
    using meta_type = read_meta<Registry, C>;

private:
    const C& val_;

public:
    constexpr read(const C& val) noexcept : val_{val}
    {}

    constexpr const C& get() const noexcept
    {
        return val_;
    }
};

template<typename Registry, typename C>
struct read_meta
{
    using required_types = C;

    using registry_type = Registry;
    using id_type       = typename registry_type::id_type;

public:
    constexpr read_meta(registry_type&) noexcept
    {}

    constexpr void process_group_vector(matter::group_vector<id_type>&) const
        noexcept
    {}

    constexpr void process_group(const_any_group<id_type>) const noexcept
    {}

    constexpr read<C>
    make_access(matter::entity_handle<id_type>     ent,
                matter::storage_handle<id_type, C> component) noexcept
    {
        return read{component[ent]};
    }

    // there are no commands to produce
    constexpr void make_commands(read<C>&& r) const noexcept
    {
        return;
    }
};

static_assert(
    matter::is_access_v<matter::read<int>, matter::registry<std::size_t>>);

// assert we're using the concept correctly
static_assert(
    matter::is_meta_access_v<read_meta<matter::registry<std::size_t>, int>>);

} // namespace matter

#endif
