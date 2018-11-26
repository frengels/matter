#ifndef MATTER_COMPONENT_VECTOR_STORAGE_HPP_INCLUDED
#define MATTER_COMPONENT_VECTOR_STORAGE_HPP_INCLUDED

#pragma once

#include <optional>
#include <vector>

namespace matter
{
template<typename Entity, typename T>
struct vector_storage
{
    using component_type = T;
    using entity_type                  = Entity;

private:
    std::vector<std::optional<component_type>> m_components;

public:
    vector_storage() = default;

    template<typename... Args>
    component_type& emplace(typename entity_type::id_type id, Args&&... args)
    {
        // enlarge if necessary
        if (std::size(m_components) <= id)
        {
            m_components.resize(id + 1, {});
        }

        m_components[id] = T(std::forward<Args>(args)...);
        return *m_components[id];
    }
};
} // namespace matter

#endif
