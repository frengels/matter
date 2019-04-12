#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "component_identifier.hpp"

#include "matter/component/group_vector.hpp"
#include "matter/component/registry_view.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Components>
class registry {
public:
    using identifier_type = component_identifier<Components...>;
    using id_type         = typename identifier_type::id_type;

private:
    identifier_type identifier_;

    /// index indicates the length of the groups stored.
    std::vector<matter::group_vector> group_vectors_;

public:
    constexpr registry() = default;

    template<typename... Cs>
    auto view() noexcept(false)
    {
        return registry_view{component_ids<Cs...>(),
                             group_vectors_.begin(),
                             group_vectors_.end()};
    }

    template<typename... TIds>
    auto view(const matter::unordered_typed_ids<id_type, TIds...>& ids) noexcept
    {
        return registry_view{ids, group_vectors_.begin(), group_vectors_.end()};
    }

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

    const matter::component_metadata& component_metadata(id_type id) noexcept
    {
        return identifier_.metadata(id);
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

        auto opt_ideal_group = find_group_from_ids(ids);

        auto ideal_group = opt_ideal_group ?
                               std::move(opt_ideal_group).value() :
                               create_group(ids);

        ideal_group.template emplace_back(std::forward<TupArgs>(args)...);
    }

    template<typename... Cs>
    auto create_buffer_for() const noexcept
    {
        return matter::insert_buffer{component_ids<Cs...>()};
    }

    template<typename... Cs, typename... TIds>
    auto create_buffer_for(
        matter::insert_buffer<matter::unordered_typed_ids<id_type, TIds...>>&&
            move_from)
    {
        return matter::insert_buffer{component_ids<Cs...>(),
                                     std::move(move_from)};
    }

    template<typename... TIds>
    void insert(const matter::insert_buffer<
                matter::unordered_typed_ids<id_type, TIds...>>&
                    buffer) noexcept((std::
                                          is_nothrow_copy_constructible_v<
                                              typename TIds::type> &&
                                      ...))
    {
        auto opt_ideal_group = find_group_from_ids(buffer.ids());

        auto ideal_group = opt_ideal_group ?
                               std::move(opt_ideal_group).value() :
                               create_group(buffer.ids());

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
    group<typename Ts::type...>
    create_group(const unordered_typed_ids<id_type, Ts...>& ids) noexcept(
        (std::is_nothrow_default_constructible_v<
             matter::component_storage_t<typename Ts::type>> &&
         ...))
    {
        assert(!find_group_from_ids(ids));

        auto& vec            = get_group_vector(ids.size());
        auto  inserted_group = vec.template emplace(ids);

        return inserted_group;
    }

    // gets the group vector for i components, automatically creates the vector
    // if it doesn't exist
    group_vector& get_group_vector(std::size_t i) noexcept
    {
        // allocate if non existant
        for (std::size_t it = group_vectors_.size(); it <= i; ++it)
        {
            group_vectors_.emplace_back(it);
        }

        auto& grp_vec = group_vectors_[i];
        assert(grp_vec.group_size() == i);
        return grp_vec;
    }

    // gets the group vector for i components, does not create the group vector
    // if it doesn't exist
    const group_vector& get_group_vector(std::size_t i) const noexcept
    {
        assert(i < group_vectors_.size());

        auto& grp_vec = group_vectors_[i];
        assert(grp_vec.group_size() == i);
        return grp_vec;
    }

    template<typename... Ts>
    constexpr std::optional<group<typename Ts::type...>> find_group_from_ids(
        const unordered_typed_ids<id_type, Ts...>& ids,
        const ordered_typed_ids<id_type, Ts...>&   ordered_ids) noexcept
    {
        auto& vec = get_group_vector(ids.size());
        return vec.find_group(ids, ordered_ids);
    }

    template<typename... Ts>
    constexpr std::optional<group<typename Ts::type...>>
    find_group_from_ids(const unordered_typed_ids<id_type, Ts...>& ids) noexcept
    {
        return find_group_from_ids(ids, ordered_typed_ids{ids});
    }

    template<typename... Cs>
    constexpr std::optional<group<Cs...>> find_group() noexcept
    {
        auto ids         = component_ids<Cs...>();
        auto ordered_ids = identifier_.template sorted_ids<Cs...>();
        return find_group_from_ids(ids, ordered_ids);
    }

    /// returns true if the vector at this index exists, the group_vector at
    /// this index can be empty. All this query indicates is that the
    /// group_vector was constructed
    bool has_groups_sized(std::size_t group_size) const noexcept
    {
        return group_size < group_vectors_.size();
    }
};

template<typename T>
struct is_registry : std::false_type
{};

template<typename... Cs>
struct is_registry<matter::registry<Cs...>> : std::true_type
{};

template<typename T>
static constexpr auto is_registry_v = is_registry<T>::value;
} // namespace matter

#endif
