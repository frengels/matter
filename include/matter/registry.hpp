#ifndef MATTER_REGISTRY_HPP
#define MATTER_REGISTRY_HPP

#pragma once

#include <tuple>
#include <type_traits>

#include "matter/component/component_traits.hpp"
#include "matter/util/type_list.hpp"

namespace matter
{
template<typename Entity, typename... Cs>
struct cx_registry
{
    using entity_type = Entity;
    std::tuple<matter::storage_type_t<entity_type, Cs>...> components_;

    template<typename C>
    constexpr bool manages() const noexcept
    {
        using component_type = std::decay_t<C>;
        return matter::detail::contains<component_type, Cs...>::value;
    }

    template<typename C>
    auto& storage() noexcept
    {
        using component_type = std::decay_t<C>;
        static_assert(manages<component_type>(),
                      "This registry doesn't manage this component");
        return std::get<matter::component_traits<component_type>::storage_type>(
            components_);
    }

    template<typename C>
    const auto& storage() const noexcept
    {
        using component_type = std::decay_t<C>;
        static_assert(manages<component_type>(),
                      "This registry doesn't manage this component");
        return std::get<matter::storage_type_t<Entity, C>>(components_);
    }
};
} // namespace matter

#endif
