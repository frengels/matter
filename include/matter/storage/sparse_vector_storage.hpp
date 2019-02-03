#ifndef MATTER_COMPONENT_COMPONENT_CONTAINER_HPP
#define MATTER_COMPONENT_COMPONENT_CONTAINER_HPP

#pragma once

#include "sparse_vector.hpp"

namespace matter
{
template<typename Id, typename T>
struct sparse_vector_storage
{
public:
    using value_type = T;
    using id_type    = Id;

private:
    using sparse_vector_type = matter::sparse_vector<T, id_type>;

public:
    using iterator         = typename sparse_vector_type::iterator;
    using const_iterator   = typename sparse_vector_type::const_iterator;
    using reverse_iterator = typename sparse_vector_type::reverse_iterator;
    using const_reverse_iterator =
        typename sparse_vector_type::const_reverse_iterator;

private:
    sparse_vector_type m_components;

public:
    sparse_vector_storage() = default;

    iterator begin() noexcept
    {
        return m_components.begin();
    }

    const_iterator begin() const noexcept
    {
        return m_components.begin();
    }

    iterator end() noexcept
    {
        return m_components.end();
    }

    const_iterator end() const noexcept
    {
        return m_components.end();
    }

    reverse_iterator rbegin() noexcept
    {
        return m_components.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return m_components.rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return m_components.rend();
    }

    const_reverse_iterator rend() const noexcept
    {
        return m_components.rend();
    }

    id_type index_of(const_iterator it) const noexcept
    {
        return m_components.index_of(it);
    }

    id_type index_of(const_reverse_iterator rit) const noexcept
    {
        return m_components.index_of(rit);
    }

    bool contains(id_type id) const noexcept
    {
        return m_components.contains(id);
    }

    value_type& operator[](id_type id) noexcept
    {
        return m_components[id];
    }

    const value_type& operator[](id_type id) const noexcept
    {
        return m_components[id];
    }

    template<typename... Args>
    void emplace(id_type id, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<value_type, Args&&...>)

    {
        m_components.emplace_back(id, std::forward<Args>(args)...);
    }

    void erase(id_type id)
    {
        m_components.erase(id);
    }
};
} // namespace matter

#endif
