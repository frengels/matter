#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "component_identifier.hpp"

#include "matter/util/id_erased.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Components>
class registry {
public:
    static constexpr std::size_t component_capacity = 64;

    static_assert(component_capacity > sizeof...(Components));

public:
    template<std::size_t I>
    class group {
    public:
        static constexpr auto num_components = I;

        using id_type = typename component_identifier<Components...>::id_type;

    private:
        std::array<matter::id_erased, num_components> storage_;

    public:
        /// \brief construct a new instance of group
        /// Constructs a group from ids + types, each forwarded as a tuple using
        /// `forward_as_tuple`. After passing the array will be sorted to ensure
        /// quick lookup for finding matching ids.
        template<typename... TupArgs>
        constexpr group(TupArgs&&... id_erased_args) noexcept
            : storage_{detail::construct_from_tuple(
                  std::in_place_type_t<matter::id_erased>{},
                  std::forward<TupArgs>(id_erased_args))...}
        {
            static_assert(sizeof...(TupArgs) <= num_components,
                          "Cannot construct with this many storages.");
            static_assert(sizeof...(TupArgs) >= num_components,
                          "Cannot construct with this few storages.");

            std::sort(storage_.begin(), storage_.end());
        }

        /// \brief checks if this group contains the passed ids
        /// Each passed id must be contained within this group, effectively the
        /// same as contains but only works on arrays with the same size as the
        /// group
        constexpr bool
        operator==(const std::array<id_type, num_components>& ids) const
            noexcept
        {
            return contains(ids);
        }

        constexpr bool
        operator!=(const std::array<id_type, num_components>& ids) const
            noexcept
        {
            return !(*this == ids);
        }

        /// \brief compares the contained ids
        /// This function can be used to sort the group in the registry's vector
        /// for this group size. However doing so is complicated and I have not
        /// started working on this so far. So these comparison functions remain
        /// mainly unused for now.
        template<std::size_t N,
                 typename = std::enable_if_t<(N <= num_components)>>
        constexpr bool operator<(const std::array<id_type, N>& ids) const
            noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            if constexpr (N == num_components)
            {
                for (std::size_t i = 0; i < storage_.size(); ++i)
                {
                    if (storage_[i].id() < ids[i])
                    {
                        return true;
                    }
                    else if (storage_[i].id() > ids[i])
                    {
                        return false;
                    }
                }

                return false;
            }
            else
            {
                auto misses = num_components - N;

                for (std::size_t i = 0, j = 0; i < storage_.size(); ++i)
                {
                    if (storage_[i].id() < ids[j])
                    {
                        // check our allowed misses left
                        if (misses == 0)
                        {
                            return true;
                        }
                        else
                        {
                            --misses;
                            continue;
                            // continue without incrementing j, checking the
                            // same array[id] next iteration
                        }
                    }
                    else if (storage_[i].id() > ids[j])
                    {
                        return false;
                    }

                    ++j;
                }

                return false;
            }
        }

        constexpr bool
        operator>(const std::array<id_type, num_components>& ids) const noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() > ids[i])
                {
                    return true;
                }
                else if (storage_[i].id() < ids[i])
                {
                    return false;
                }
            }

            return false;
        }

        constexpr bool operator<(const group& other) const noexcept
        {
            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() < other.storage_[i].id())
                {
                    return true;
                }
                else if (storage_[i].id() > other.storage_[i].id())
                {
                    return false;
                }
            }

            // when equal we get here
            return false;
        }

        constexpr bool operator>(const group& other) const noexcept
        {
            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() > other.storage_[i].id())
                {
                    return true;
                }
                else if (storage_[i].id() < other.storage_[i].id())
                {
                    return false;
                }
            }

            // when equal we get here
            return false;
        }

        constexpr bool contains(id_type id) const noexcept
        {
            auto index_it = find_id(id);
            return index_it != storage_.end();
        }

        template<std::size_t N,
                 typename = std::enable_if_t<(N <= num_components)>>
        constexpr bool contains(const std::array<id_type, N>& ids) const
            noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            return std::includes(
                storage_.begin(), storage_.end(), ids.begin(), ids.end());
        }

        template<typename C>
        matter::component_storage_t<C>& get(id_type id) noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->template get<matter::component_storage_t<C>>();
        }

        template<typename C>
        const matter::component_storage_t<C>& get(id_type id) const noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->template get<matter::component_storage_t<C>>();
        }

    private:
        /// \brief checks whether the stores are sorted
        constexpr auto is_sorted() const noexcept
        {
            return std::is_sorted(storage_.begin(), storage_.end());
        }

        constexpr auto find_id(id_type id) noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(), storage_.end(), id);

            if (index_it->id() != id)
            {
                return storage_.end();
            }

            return index_it;
        }

        constexpr auto find_id(id_type id) const noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(), storage_.end(), id);

            if (index_it->id() != id)
            {
                return storage_.end();
            }

            return index_it;
        }
    };

    template<template<std::size_t> typename T, typename IndexSeq>
    struct indexed_tuple;

    template<template<std::size_t> typename T, std::size_t... N>
    struct indexed_tuple<T, std::index_sequence<N...>>
    {
        using type = std::tuple<T<N>...>;
    };

public:
    using identifier_type = component_identifier<Components...>;
    using id_type         = typename identifier_type::id_type;

private:
    template<std::size_t I>
    using group_vector = std::vector<group<I>>;

private:
    identifier_type identifier_;

    typename indexed_tuple<group_vector,
                           std::make_index_sequence<component_capacity>>::type
        groups_;

public:
    constexpr registry() = default;

    template<typename C>
    constexpr id_type component_id() const noexcept
    {
        return identifier_.template id<C>();
    }

    template<typename C>
    void register_component() noexcept
    {
        identifier_.template register_type<C>();
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

        auto ids        = identifier_.template ids<Cs...>();
        auto sorted_ids = ids;
        std::sort(sorted_ids.begin(), sorted_ids.end());

        auto* ideal_group = find_group_from_ids(sorted_ids);

        if (!ideal_group)
        {
            // no group exists for these components so create one
            ideal_group = create_group<Cs...>();
        }

        (create_impl<Cs>(*ideal_group, ids, std::forward<TupArgs>(args)), ...);
    }

private:
    template<typename C, std::size_t I, std::size_t N, typename... Args>
    void create_impl(group<I>&                     grp,
                     const std::array<id_type, N>& ids,
                     std::tuple<Args...>           args) noexcept
    {
        create_impl_impl<C>(
            grp, std::index_sequence_for<Args...>{}, ids, std::move(args));
    }

    template<typename C,
             std::size_t I,
             std::size_t N,
             std::size_t... Is,
             typename... Args>
    void create_impl_impl(group<I>& grp,
                          std::index_sequence<Is...>,
                          const std::array<id_type, N>& ids,
                          std::tuple<Args...>           args) noexcept
    {
        grp.template get<C>(ids[I]).emplace_back(
            std::move(std::get<Is>(args))...);
    }

    template<typename... Cs>
    group<sizeof...(Cs)>* create_group() noexcept
    {
        constexpr auto I          = sizeof...(Cs);
        auto           ids        = identifier_.template ids<Cs...>();
        auto           sorted_ids = ids;
        std::sort(sorted_ids.begin(), sorted_ids.end());
        assert(find_group_from_ids(sorted_ids) == nullptr);

        auto& vec = get_group_vector<I>();
        auto  it  = std::lower_bound(vec.begin(), vec.end(), sorted_ids);

        using seq = std::make_index_sequence<I>;
        return create_group_impl<Cs...>(seq{}, ids, vec, it);
    }

    template<typename... Cs, std::size_t... Indices>
    group<sizeof...(Cs)>* create_group_impl(
        std::index_sequence<Indices...>,
        const std::array<id_type, sizeof...(Cs)>&            ids,
        group_vector<sizeof...(Cs)>&                         vec,
        typename group_vector<sizeof...(Cs)>::const_iterator it) noexcept
    {
        static_assert(sizeof...(Cs) == sizeof...(Indices),
                      "Cannot pass more types than indices");
        constexpr auto I = sizeof...(Cs);

        auto inserted_it = vec.insert(
            it,
            group<I>{std::forward_as_tuple(
                ids[Indices],
                std::in_place_type_t<matter::component_storage_t<Cs>>{})...});

        return std::addressof(*inserted_it);
    }

    template<std::size_t I>
    group_vector<I>& get_group_vector() noexcept
    {
        return std::get<group_vector<I>>(groups_);
    }

    template<std::size_t I>
    const group_vector<I>& get_group_vector() const noexcept
    {
        return std::get<group_vector<I>>(groups_);
    }

    template<std::size_t I>
    constexpr group<I>*
    find_group_from_ids(const std::array<id_type, I>& sorted_ids) noexcept
    {
        assert(std::is_sorted(sorted_ids.begin(), sorted_ids.end()));
        auto& vec = get_group_vector<I>();

        auto it = std::lower_bound(vec.begin(), vec.end(), sorted_ids);

        if (it == vec.end())
        {
            return nullptr;
        }

        if (*it == sorted_ids)
        {
            return &(*it);
        }

        return nullptr;
    }

    template<typename... Cs>
    group<sizeof...(Cs)>* find_group() noexcept
    {
        auto sorted_ids = identifier_.template sorted_ids<Cs...>();
        return find_group_from_ids(sorted_ids);
    }
};

} // namespace matter

#endif
