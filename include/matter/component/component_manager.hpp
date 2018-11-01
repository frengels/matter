#ifndef MATTER_COMPONENT_COMPONENT_MANAGER_HPP
#define MATTER_COMPONENT_COMPONENT_MANAGER_HPP

#pragma once

#include <cassert>
#include <memory>

#include "matter/component/component_storage.hpp"
#include "matter/component/identifier.hpp"

namespace matter
{
namespace detail
{
struct component_tag
{};
} // namespace detail

template<typename Entity>
struct component_manager
{
    using entity_type = Entity;
    using id_type     = typename entity_type::id_type;

private:
    /// type erased list of component arrays
    std::vector<std::unique_ptr<void>> m_component_list;

public:
    template<typename C>
    matter::component_storage<entity_type, C>& get_storage() noexcept
    {
        auto id = component_id<C>();
        assert(id < std::size(m_component_list));

        auto& comp_storage =
            *static_cast<matter::component_storage<entity_type, C>*>(
                m_component_list[id].get());
        return comp_storage;
    }

    template<typename C>
    const matter::component_storage<entity_type, C>& get_storage() const
        noexcept
    {
        auto id = component_id<C>();
        assert(id < std::size(m_component_list));

        const auto& comp_storage =
            *static_cast<matter::component_storage<entity_type, C>*>(
                m_component_list[id].get());
        return comp_storage;
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
            m_component_list.emplace_back(std::unique_ptr<void>(
                new matter::component_storage<entity_type, C>{}, [](void* p) {
                    auto* storage =
                        static_cast<matter::component_storage<entity_type, C>>(
                            p);
                    delete storage;
                }));
            assert(id < std::size(m_component_list) &&
                   "forgot to insert a component storage");
        }

	return id;
    }

    template<typename C>
    matter::component_storage<entity_type, C>& storage() noexcept
    {
        auto id = create_storage_if_null<C>();
        return *static_cast<matter::component_storage<entity_type, C>*>(
            m_component_list[id].get());
    }
};
} // namespace matter

#endif
