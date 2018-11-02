#ifndef MATTER_ENTITY_ENTITY_MANAGER_HPP
#define MATTER_ENTITY_ENTITY_MANAGER_HPP

#pragma once

#include <cassert>
#include <queue>
#include <vector>

#include "matter/entity/entity_traits.hpp"

namespace matter
{
template<typename Entity>
struct entity_manager
{
    static_assert(matter::is_entity_v<Entity>, "Entity is not a valid entity");

    using entity_type  = Entity;
    using id_type      = typename entity_type::id_type;
    using generation_type = typename entity_type::generation_type;

private:
    std::vector<generation_type> m_entities;
    std::queue<id_type>       m_free_entities;

public:
    entity_manager()  = default;
    ~entity_manager() = default;

    entity_type create()
    {
        id_type      id;
        generation_type ver;
        if (!std::empty(m_free_entities))
        {
            id = std::move(m_free_entities.front());
            m_free_entities.pop();
            ver = current_generation(id);
        }
        else
        {
            // initialize with version 0
            ver = 0;
	    id = static_cast<id_type>(std::size(m_entities));
            m_entities.push_back(ver);
        }

        return entity_type{id, ver};
    }

    void destroy(const entity_type& ent)
    {
        // assert a valid context
        assert(ent.id() <= std::size(m_entities));
        // check we're trying to destroy a current entity
        assert(ent.generation() == current_generation(ent.id()));

        ++current_generation(ent.id());
        m_free_entities.push(ent.id());
    }

    bool is_valid(const entity_type& ent) const
    {
        return current_generation(ent.id()) == ent.generation();
    }

    generation_type& current_generation(const id_type& id)
    {
        return m_entities[id];
    }

    const generation_type& current_generation(const id_type& id) const
    {
        return m_entities[id];
    }

    size_t size() const noexcept
    {
        return std::size(m_entities) - std::size(m_free_entities);
    }

    size_t capacity() const noexcept
    {
        return m_entities.capacity();
    }
};
} // namespace matter

#endif
