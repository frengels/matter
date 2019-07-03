#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "matter/id/component_identifier.hpp"

#include "matter/component/group_container.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Identifier>
class registry {
    static_assert(matter::is_component_identifier_v<Identifier>);

public:
    using identifier_type      = Identifier;
    using id_type              = typename identifier_type::id_type;
    using group_container_type = matter::group_container<id_type>;

private:
    identifier_type identifier_{};

    group_container_type container_;

public:
    constexpr registry() noexcept = default;

    template<typename C>
    constexpr std::enable_if_t<
        matter::is_component_identifier_for_v<identifier_type, C>,
        matter::typed_id<id_type, C>>
    component_id() const
        noexcept(noexcept(identifier_.template component_id<C>()))
    {
        return identifier_.template component_id<C>();
    }

    template<typename... Cs>
    constexpr std::enable_if_t<
        (matter::is_component_identifier_for_v<identifier_type, Cs> && ...),
        matter::unordered_typed_ids<id_type, Cs...>>
    component_ids() const
    {
        return matter::unordered_typed_ids{component_id<Cs>()...};
    }

    template<typename C>
    constexpr std::enable_if_t<
        matter::is_dynamic_component_identifier_v<identifier_type>,
        matter::typed_id<id_type, C>>
    register_component() noexcept(
        noexcept(identifier_.template register_component<C>()))
    {
        return identifier_.template register_component<C>();
    }

    template<typename C>
    constexpr bool contains_component() const
        noexcept(noexcept(identifier_.template contains_component<C>()))
    {
        return identifier_.template contains_component<C>();
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
    void create(TupArgs&&... args)
    {
        static_assert(sizeof...(Cs) == sizeof...(TupArgs),
                      "Did not provide Component for each Argument.");

        auto ids = component_ids<Cs...>();

        auto ideal_group = try_emplace_group(ids);

        static_assert(boost::hana::is_valid([&]() {
                          ideal_group.template emplace_back(
                              std::forward<TupArgs>(args)...);
                      })(),
                      "cannot emplace into container from provided arguments");

        // this emplace_back can emplace from tuple as well as non tuple
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
