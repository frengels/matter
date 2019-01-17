#ifndef MATTER_REGISTRY_HPP
#define MATTER_REGISTRY_HPP

#pragma once

#include <tuple>
#include <type_traits>

#include "component/component_traits.hpp"
#include "util/crtp.hpp"
#include "util/type_list.hpp"

namespace matter
{
template<typename R>
struct registry
{
public:
    registry()
    {
        static_assert(matter::is_crtp_of<R, registry>::value,
                      "registry is a crtp class");
    }

private:
    const R& impl() const noexcept
    {
        return *static_cast<R*>(this);
    }

    R& impl() noexcept
    {
        return *static_cast<R*>(this);
    }
};

template<typename Entity, typename... Cs>
struct cx_registry : registry<cx_registry<Entity, Cs...>>
{
    using entity_type = Entity;

private:
    std::tuple<matter::storage_type_t<entity_type, Cs>...> components_;

public:
    cx_registry() = default;

    template<typename... Ts>
    constexpr bool manages() const noexcept
    {
        return (manages_impl<Ts>() && ...);
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

private:
    template<typename T>
    constexpr bool manages_impl() const noexcept
    {
        return matter::detail::contains<
            std::remove_reference_t<std::remove_cv_t<T>>,
            Cs...>::value;
    }
};
} // namespace matter

#endif
