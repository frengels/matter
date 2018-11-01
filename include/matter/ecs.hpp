#ifndef MATTER_ECS_HPP
#define MATTER_ECS_HPP

#pragma once

#include <queue>
#include <tuple>
#include <type_traits>
#include <vector>

#include "matter/entity/entity.hpp"
#include "matter/entity/entity_handle.hpp"
#include "matter/entity/entity_manager.hpp"

namespace matter
{
namespace detail
{
template<typename T, template<typename...> typename Template>
struct is_specialization_of : public std::false_type
{};
template<template<typename...> typename Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : public std::true_type
{};

template<typename Tuple>
struct is_tuple : public is_specialization_of<
                      std::remove_cv_t<std::remove_reference_t<Tuple>>,
                      std::tuple>
{};

template<typename... Tuples>
struct are_tuples
    : public std::integral_constant<bool, (is_tuple<Tuples>::value && ...)>
{};

template<typename T, typename Tuple>
struct is_constructible_tuple;
template<typename T, typename... Args>
struct is_constructible_tuple<T, std::tuple<Args...>>
    : public std::is_constructible<T, Args...>
{};

template<typename Ts, typename TupleArgs>
struct are_constructible;
template<typename... Ts, typename... TupleArgs>
struct are_constructible<std::tuple<Ts...>, std::tuple<TupleArgs...>>
    : public std::integral_constant<
          bool,
          (is_constructible_tuple<Ts, TupleArgs>::value && ...)>
{};

template<typename F, std::size_t... Is>
void repeat_impl(F&& f, std::index_sequence<Is...>)
{
    (f(std::integral_constant<decltype(Is), Is>{}), ...);
}

template<auto N, typename F>
void repeat(F&& f)
{
    repeat_impl(std::forward<F>(f), std::make_index_sequence<N>{});
}

template<typename ComponentManager,
         std::size_t... Is,
         typename T,
         typename... Args>
auto emplace_component_from_tuple_impl(
    ComponentManager&                     manager,
    typename ComponentManager::index_type idx,
    std::tuple<Args...>                   args,
    std::index_sequence<Is...>) noexcept -> decltype(auto)
{
    return manager.template emplace<T>(idx, std::get<Is>(args)...);
}

template<typename ComponentManager, typename T, typename... Args>
auto emplace_component_from_tuple(
    ComponentManager&                     manager,
    typename ComponentManager::index_type idx,
    std::tuple<Args...>
        args) noexcept(noexcept(emplace_component_from_tuple_impl(manager,
                                                                  idx,
                                                                  args,
                                                                  std::index_sequence_for<
                                                                      Args...>{})))
    -> decltype(auto)
{
    // return manager.template emplace<T>(idx, args...);
    return emplace_component_from_tuple_impl(
        manager, idx, args, std::index_sequence_for<Args...>{});
}

} // namespace detail

template<typename Entity>
class ecs {
public:
    using entity_manager_type = matter::entity_manager<Entity>;
    using entity_type         = typename entity_manager_type::entity_type;

private:
    entity_manager_type m_entities;

    struct
    {
        using index_type = typename entity_type::id_type;
        template<typename T, typename... Args>
        T& emplace(index_type idx, Args&&... args)
        {
            static_assert(std::is_constructible_v<T, Args&&...>,
                          "T is not constructible from Args&&...");
        }
    } m_components;

public:
    ecs() = default;

private:
public:
    /*!
     * ecs.create_entity<Mesh, RigidBody, Transform>(std::forward_as_tuple(...),
     * std::forward_as_tuple(...), std::forward_as_tuple(...));
     */
    template<typename... Cs, typename... CsArgs>
    matter::entity_handle<entity_type> create_entity(CsArgs... comp_args)
    {
        static_assert(sizeof...(Cs) == sizeof...(CsArgs),
                      "You must pass a forwarded tuple argument pack for each "
                      "component you wish to construct");
        static_assert(
            detail::are_tuples<CsArgs...>::value,
            "All Component constructor arguments must be forwarded as a tuple");
        static_assert(
            detail::are_constructible<std::tuple<Cs...>,
                                      std::tuple<CsArgs...>>::value,
            "Not all components are constructible with the provided arguments");

        auto ent = m_entities.create();

        auto tupled_comp_args = std::forward_as_tuple(comp_args...);

        detail::repeat<sizeof...(Cs)>([&](auto i) {
            detail::emplace_component_from_tuple(
                m_components,
                ent.id(),
                std::get<decltype(i)::value>(tupled_comp_args));
        });

        return matter::entity_handle<entity_type>{std::move(ent), *this};
    }
};
} // namespace matter

#endif
