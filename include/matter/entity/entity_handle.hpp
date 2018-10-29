#ifndef MATTER_ENTITY_ENTITY_HANDLE_HPP
#define MATTER_ENTITY_ENTITY_HANDLE_HPP

#pragma once

#include <functional>

namespace matter
{
template<typename Entity>
class ecs;

template<typename Entity>
struct entity_handle
{
    using entity_type = Entity;
    using ecs_type    = matter::ecs<entity_type>;

private:
    entity_type                      m_entity;
    std::reference_wrapper<ecs_type> m_ecs;

public:
    constexpr entity_handle(entity_type&& ent, matter::ecs<entity_type>& ecs)
        : m_entity{std::move(ent)}, m_ecs{ecs}
    {}

    constexpr const entity_type& get() const noexcept
    {
        return m_entity;
    }

    constexpr entity_type& get() noexcept
    {
        return m_entity;
    }

    constexpr const ecs_type& ecs() const noexcept
    {
        return m_ecs;
    }

    constexpr ecs_type& ecs() noexcept
    {
        return m_ecs;
    }
};
} // namespace matter

#endif
