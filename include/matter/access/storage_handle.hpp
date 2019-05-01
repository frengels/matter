#ifndef MATTER_ACCESS_STORAGE_HANDLE_HPP
#define MATTER_ACCESS_STORAGE_HANDLE_HPP

#pragma once

#include <functional>

#include "matter/access/entity_handle.hpp"

namespace matter
{
template<typename Id, typename Component>
class storage_handle {
    static_assert(
        std::is_same_v<Component, std::remove_const_t<Component>>,
        "const type are not allowed, most containers don't allow these");

public:
    using id_type = Id;

private:
    std::reference_wrapper<matter::component_storage_t<Component>> store_;

public:
    constexpr storage_handle(
        matter::component_storage_t<Component>& store) noexcept
        : store_{store}
    {}

    constexpr Component&
    operator[](const matter::entity_handle<id_type>& ent) noexcept
    {
        return store_.get()[ent.index()];
    }

    constexpr const Component&
    operator[](const matter::entity_handle<id_type>& ent) const noexcept
    {
        return store_[ent.index()];
    }
};
} // namespace matter

#endif
