#ifndef MATTER_COMPONENT_COMPONENT_MANAGER_HPP
#define MATTER_COMPONENT_COMPONENT_MANAGER_HPP

#pragma once

#include <cassert>
#include <memory>

#include "matter/component/component_traits.hpp"
#include "matter/component/identifier.hpp"
#include "matter/component/sparse_vector_storage.hpp"

namespace matter
{
namespace detail
{
struct component_tag
{};

struct component_container_ptr
{
    using deleter_type = std::add_pointer_t<void(void*)>;
    void*        ptr;
    deleter_type deleter;

    component_container_ptr(void* cont, deleter_type deleter)
        : ptr{cont}, deleter{deleter}
    {}

    template<typename Container>
    component_container_ptr(Container* cont, deleter_type deleter)
        : component_container_ptr{static_cast<void*>(cont), deleter}
    {}

    component_container_ptr(const component_container_ptr&) noexcept = delete;
    component_container_ptr&
    operator=(const component_container_ptr&) noexcept = delete;

    component_container_ptr(component_container_ptr&& other) noexcept
        : ptr{std::move(other.ptr)}, deleter{std::move(other.deleter)}
    {
        other.ptr     = nullptr;
        other.deleter = nullptr;
    }
    component_container_ptr& operator=(component_container_ptr&& other) noexcept
    {
        std::swap(ptr, other.ptr);
        std::swap(deleter, other.deleter);

        return *this;
    }

    ~component_container_ptr()
    {
        if (ptr)
        {
            deleter(ptr);
        }
    }
};
} // namespace detail

template<typename Entity>
struct component_manager
{
    using entity_type = Entity;
    using id_type     = typename entity_type::id_type;

private:
    /// type erased list of component arrays
    std::vector<detail::component_container_ptr> m_component_list;

public:
    template<typename C>
    auto& storage() noexcept
    {
        using storage_type =
            matter::component_traits<C>::storage_type<entity_type>;
        auto id = create_storage_if_null<C>();
        return *static_cast<storage_type*>(
            m_component_list[id].ptr);
    }

private:
    template<typename C>
    auto component_id() const noexcept -> decltype(auto)
    {
        return matter::identifier<detail::component_tag>::get<C>();
    }

    template<typename C>
    auto create_storage_if_null() -> decltype(auto)
    {
        auto id = component_id<C>();
        if (id >= std::size(m_component_list)) // create the storage
        {
            using storage_type =
                matter::component_traits<C>::storage_type<entity_type>;
            m_component_list.emplace_back(new storage_type(), [](void* storage) {
                auto* real_storage = static_cast<storage_type*>(storage);
                delete real_storage;
            });
        }
        assert(id < std::size(m_component_list) &&
               "forgot to insert a component storage");

        return id;
    }
};
} // namespace matter

#endif
