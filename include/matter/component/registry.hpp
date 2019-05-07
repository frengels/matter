#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "component_identifier.hpp"

#include "matter/access/registry_access_view.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/component/group_container.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Id, typename... Components>
class registry {
    static_assert(matter::is_id_v<Id>);
    static_assert((matter::is_component_v<Components> && ...));

public:
    using identifier_type      = component_identifier<Id, Components...>;
    using id_type              = typename identifier_type::id_type;
    using group_container_type = matter::group_container<id_type>;

private:
    identifier_type identifier_{};

    group_container_type container_;

public:
    constexpr registry() noexcept = default;

    template<typename C>
    constexpr auto component_id() const
    {
        return identifier_.template id<C>();
    }

    template<typename... Cs>
    constexpr auto component_ids() const
    {
        return identifier_.template ids<Cs...>();
    }

    template<typename C>
    void register_component() noexcept
    {
        identifier_.template register_type<C>();
    }

    template<typename... Access>
    auto view() noexcept
    {
        return registry_access_view<matter::registry<id_type, Components...>,
                                    Access...>{*this};
    }

    group_container_type& group_container() noexcept
    {
        return container_;
    }

    const group_container_type& group_container() const noexcept
    {
        return container_;
    }

    template<typename... Cs, typename... TupArgs>
    void create(TupArgs&&... args) noexcept(
        (detail::is_nothrow_constructible_expand_tuple_v<Cs, TupArgs> && ...))
    {
        static_assert(sizeof...(Cs) == sizeof...(TupArgs),
                      "Did not provide arguments for each Component.");
        static_assert(
            (detail::is_constructible_expand_tuple_v<Cs, TupArgs> && ...),
            "One of the components cannot be constructed from the provided "
            "args.");

        auto ids = identifier_.template ids<Cs...>();

        auto ideal_group = try_emplace_group(ids);

        ideal_group.template emplace_back(std::forward<TupArgs>(args)...);
    }

    template<typename... Cs>
    auto create_buffer_for() const noexcept
    {
        return matter::insert_buffer{component_ids<Cs...>()};
    }

    template<typename... Cs, typename... Ts>
    auto create_buffer_for(matter::insert_buffer<id_type, Ts...>&& move_from)
    {
        return matter::insert_buffer{component_ids<Cs...>(),
                                     std::move(move_from)};
    }

    template<typename... Ts>
    void insert(const matter::insert_buffer<id_type, Ts...>& buffer) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> && ...))
    {
        auto ideal_group = try_emplace_group(buffer.ids());
        ideal_group.insert_back(buffer);
    }

    template<typename GroupViewIterator>
    void erase(GroupViewIterator it, std::size_t idx) noexcept
    {
        // need to use the very abstract GroupViewIterator because otherwise
        // template deduction doesn't work at all
        auto grp = (*it).underlying_group();
        grp.erase(idx);
    }

private:
    template<typename... Ts>
    constexpr matter::group<id_type, Ts...> try_emplace_group(
        const matter::unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return container_.try_emplace_group(ids);
    }

    constexpr any_group<id_type>
    find_emplace_group(const_any_group<id_type>             storage_source,
                       matter::ordered_untyped_ids<id_type> new_ids) noexcept
    {
        auto& grp_vec = get_group_vector(new_ids.size());
        return grp_vec.find_emplace_group(std::move(storage_source),
                                          std::move(new_ids));
    }

    template<typename... Ts>
    constexpr std::optional<group<id_type, Ts...>>
    find_group(const unordered_typed_ids<id_type, Ts...>& ids,
               const ordered_typed_ids<id_type, Ts...>&   ordered_ids) noexcept
    {
        return container_.find_group(ids, ordered_ids);
    }

    template<typename... Ts>
    constexpr std::optional<group<id_type, Ts...>>
    find_group(const unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return find_group(ids, matter::ordered_typed_ids{ids});
    }

    template<typename... Cs>
    constexpr std::optional<group<id_type, Cs...>> find_group() noexcept
    {
        return find_group(component_ids<Cs...>());
    }
};
} // namespace matter

#endif
