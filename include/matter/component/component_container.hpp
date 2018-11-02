#ifndef MATTER_COMPONENT_COMPONENT_CONTAINER_HPP
#define MATTER_COMPONENT_COMPONENT_CONTAINER_HPP

#pragma once

#include "matter/component/sparse_vector.hpp"

namespace matter
{
template<typename Entity, typename T>
struct component_container
{
public:
    using component_type = T;
    using entity_type    = Entity;
    using sparse_vector_type =
        matter::sparse_vector<T, typename entity_type::id_type>;

    using value_type = typename sparse_vector_type::value_type;

private:
    sparse_vector_type m_components;

public:
    component_container() = default;

    template<typename... Args>
    component_type&
    emplace(const typename entity_type::id_type& id, Args&&... args) noexcept(
        noexcept(component_type(std::declval<Args&&>()...)))
    {
        return m_components.emplace_back(id, std::forward<Args>(args)...);
    }


};
} // namespace matter

#endif
