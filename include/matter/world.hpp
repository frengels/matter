#ifndef MATTER_WORLD_HPP
#define MATTER_WORLD_HPP

#include "matter/component/registry.hpp"
#include "matter/id/default_component_identifier.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Registry = matter::registry<
             matter::default_component_identifier<matter::signed_id<int>>>>
class world {
public:
    using registry_type   = Registry;
    using identifier_type = typename registry_type::identifier_type;
    using id_type         = typename identifier_type::id_type;

private:
    registry_type registry_;

public:
    world() noexcept = default;

    template<typename T>
    constexpr auto component_id() const
        noexcept(noexcept(registry_.template component_id<T>()))
            -> std::enable_if_t<
                matter::is_component_identifier_for_v<identifier_type, T>,
                decltype(registry_.template component_id<T>())>
    {
        return registry_.template component_id<T>();
    }

    template<typename T>
    constexpr bool contains_component() const
        noexcept(noexcept(registry_.template contains_component<T>()))
    {
        return registry_.template contains_component<T>();
    }

    template<typename T>
    constexpr auto register_component() noexcept(
        noexcept(registry_.template register_component<T>()))
        -> std::enable_if_t<
            matter::is_dynamic_component_identifier_v<identifier_type>,
            decltype(registry_.template register_component<T>())>
    {
        return registry_.template register_component<T>();
    }

    decltype(auto) group_range()
    {
        return registry_.group_container().range();
    }

    /// create an entity composed of the given components
    /// TODO add a handle to the entity as a return value
    template<typename... Ts, typename... Args>
    constexpr decltype(auto) create_entity(Args&&... args) noexcept(
        noexcept(registry_.template create<Ts...>(std::forward<Args>(args)...)))
    {
        registry_.template create<Ts...>(std::forward<Args>(args)...);
    }
};
} // namespace matter

#endif
