#ifndef MATTER_ACCESS_PROTOTYPE_HPP
#define MATTER_ACCESS_PROTOTYPE_HPP

#pragma once

#include "matter/access/type_traits.hpp"
#include "matter/component/registry.hpp"

namespace matter
{
namespace prototype
{
template<typename Registry>
struct meta_access;

struct access
{
    template<typename Registry>
    using meta_type = matter::prototype::meta_access<Registry>;
};

template<typename Registry>
struct meta_access
{
    using required_types = void;
    using registry_type  = Registry;

    constexpr meta_access(registry_type&);

    constexpr void process_group_vector(matter::group_vector&);

    constexpr void process_group(matter::any_group);

    constexpr void make_access(matter::entity_handle);
};

template<typename Registry>
struct meta_access_other
{
    using required_types = std::tuple<int>;
    using registry_type  = Registry;

    constexpr meta_access_other(registry_type&);

    constexpr std::optional<float> process_group_vector(matter::group_vector&);

    // take any group and the derefed result of process_group_vector if not void
    constexpr std::optional<char> process_group(matter::any_group, float);

    // this function gets the handle, the dereferenced return value of
    // process_group, or process_group_vector if process_group function is not
    // present. Additionaly it obtains a reference to the types mentioned in
    // required_types.
    // required_types is also used in filtering groups for presence.
    constexpr matter::prototype::access
    make_access(matter::entity_handle, char, int&);
};

static_assert(
    matter::is_access_v<matter::prototype::access, matter::registry<>>);
static_assert(matter::is_meta_access_v<
              matter::prototype::meta_access<matter::registry<>>>);
static_assert(matter::is_meta_access_v<
              matter::prototype::meta_access_other<matter::registry<>>>);

static_assert(
    std::is_same_v<std::tuple<>,
                   matter::required_types_t<
                       matter::prototype::meta_access<matter::registry<>>>>);
} // namespace prototype
} // namespace matter

#endif
