#ifndef MATTER_COMPONENT_REGISTRY_VIEW_HPP
#define MATTER_COMPONENT_REGISTRY_VIEW_HPP

#pragma once

#include "matter/component/group_vector_view.hpp"
#include "matter/component/group_view.hpp"
#include "matter/component/registry.hpp"

namespace matter
{

template<typename Registry, typename UnorderedTypedIds>
struct registry_view;

template<typename Registry, typename... TIds>
struct registry_view<
    Registry,
    matter::unordered_typed_ids<typename Registry::id_type, TIds...>>
{
    static_assert(matter::is_registry_v<Registry>,
                  "Registry is not of type registry");

public:
    using unordered_ids_type =
        matter::unordered_typed_ids<typename Registry::id_type, TIds...>;
    using registry_type = Registry;
    using id_type       = typename Registry::id_type;

private:
    matter::unordered_typed_ids<id_type, TIds...> ids_;
    std::reference_wrapper<registry_type>         registry_;

public:
    constexpr registry_view(const unordered_ids_type& ids, Registry& reg)
        : ids_{ids}, registry_{reg}
    {}

    template<typename Function>
    void for_each(Function f) noexcept(
        std::is_nothrow_invocable_v<Function, typename TIds::type&...>)
    {
        auto beg = registry_.get().group_vectors().begin() +
                   unordered_ids_type::size();

        if (beg >= registry_.get().group_vectors().end())
        {
            return;
        }

        std::for_each(
            beg,
            registry_.get().group_vectors().end(),
            [ids = ids_, f = std::move(f)](group_vector& grp_vec) {
                matter::group_vector_view view{ids, grp_vec};
                std::for_each(
                    view.begin(),
                    view.end(),
                    [f = std::move(f), size = grp_vec.group_size(), ids](
                        auto& erased) {
                        auto grp      = group{erased, size};
                        auto grp_view = matter::group_view{ids, grp};

                        std::for_each(grp_view.begin(),
                                      grp_view.end(),
                                      [f = std::move(f)](auto comp_view) {
                                          comp_view.invoke(f);
                                      });
                    });
            });
    }
};

template<typename Registry, typename... TIds>
registry_view(
    const matter::unordered_typed_ids<typename Registry::id_type, TIds...>,
    Registry& reg)
    ->registry_view<
        Registry,
        matter::unordered_typed_ids<typename Registry::id_type, TIds...>>;
} // namespace matter

#endif
