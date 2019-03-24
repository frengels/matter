#ifndef MATTER_COMPONENT_REGISTRY_VIEW_HPP
#define MATTER_COMPONENT_REGISTRY_VIEW_HPP

#pragma once

#include "matter/component/group_vector_view.hpp"
#include "matter/component/group_view.hpp"

namespace matter
{
template<typename... Components>
class registry;

template<typename UnorderedTypedIds>
struct registry_view;

template<typename Id, typename... TIds>
struct registry_view<matter::unordered_typed_ids<Id, TIds...>>
{
    using id_type            = Id;
    using unordered_ids_type = matter::unordered_typed_ids<id_type, TIds...>;

private:
    matter::unordered_typed_ids<id_type, TIds...> ids_;
    std::vector<group_vector>::iterator           begin_;
    std::vector<group_vector>::iterator           end_;

public:
    constexpr registry_view(const unordered_ids_type&           ids,
                            std::vector<group_vector>::iterator begin,
                            std::vector<group_vector>::iterator end) noexcept
        : ids_{ids}, begin_{begin + unordered_ids_type::size()}, end_{end}
    {}

    template<typename Function>
    void for_each(Function f) noexcept(
        std::is_nothrow_invocable_v<Function, typename TIds::type&...>)
    {
        if (begin_ >= end_)
        {
            return;
        }

        std::for_each(
            begin_,
            end_,
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
                                          comp_view.invoke(std::move(f));
                                      });
                    });
            });
    }
};

template<typename Id, typename... TIds>
registry_view(const matter::unordered_typed_ids<Id, TIds...>,
              std::vector<group_vector>::iterator begin,
              std::vector<group_vector>::iterator end)
    ->registry_view<matter::unordered_typed_ids<Id, TIds...>>;
} // namespace matter

#endif
