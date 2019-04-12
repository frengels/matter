#ifndef MATTER_COMPONENT_REGISTRY_VIEW_HPP
#define MATTER_COMPONENT_REGISTRY_VIEW_HPP

#pragma once

#include "matter/component/group_vector_view.hpp"
#include "matter/component/group_view.hpp"
#include "matter/util/algorithm.hpp"

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

    struct iterator
    {
        using group_vector_container_type = std::vector<group_vector>;
        using group_vector_container_iterator_type =
            matter::iterator_t<group_vector_container_type>;
        using group_vector_container_sentinel_type =
            matter::sentinel_t<group_vector_container_type>;

        using group_vector_view_type = decltype(matter::group_vector_view{
            std::declval<unordered_ids_type>(),
            *std::declval<group_vector_container_iterator_type>()});
        using group_vector_view_iterator_type =
            matter::iterator_t<group_vector_view_type>;
        using group_vector_view_sentinel_type =
            matter::sentinel_t<group_vector_view_type>;

        using value_type        = group_view<typename TIds::type...>;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = std::forward_iterator_tag;

    private:
        group_vector_container_iterator_type grp_vec_container_it_;
        group_vector_container_sentinel_type grp_vec_container_sent_;

        group_vector_view_iterator_type grp_vec_view_it_;
        group_vector_view_sentinel_type grp_vec_view_sent_;

    public:
        constexpr iterator(
            const unordered_ids_type&            ids,
            group_vector_container_iterator_type begin_range,
            group_vector_container_sentinel_type end_range) noexcept
            : grp_vec_container_it_{begin_range + ids.size()},
              grp_vec_container_sent_{end_range}, grp_vec_view_it_{[&]() {
                  auto grp_vec_view =
                      group_vector_view_type{ids, *grp_vec_container_it_};
                  return grp_vec_view.begin();
              }()},
              grp_vec_view_sent_{
                  typename group_vector_view<unordered_ids_type>::sentinel{}}
        {

            if (grp_vec_view_it_ != grp_vec_view_sent_)
            {
                // the group_vector_view is not empty, hooray
                return;
            }
            while (++grp_vec_container_it_ < grp_vec_container_sent_)
            {
                auto grp_vec_view = group_vector_view_type{
                    grp_vec_view_it_.ids(), *grp_vec_container_it_};
                grp_vec_view_it_   = grp_vec_view.begin();
                grp_vec_view_sent_ = grp_vec_view.end();

                if (grp_vec_view_it_ != grp_vec_view_sent_)
                {
                    // finally a non empty group_vector_view, after months of
                    // iterations
                    return;
                }
            }
        }

        constexpr iterator& operator++() noexcept
        {
            // we hit the end of our current group_vector_view, let's move up
            if (++grp_vec_view_it_ == grp_vec_view_sent_)
            {
                do
                {
                    if (++grp_vec_container_it_ == grp_vec_container_sent_)
                    {
                        // we hit the end, nothing left to do
                        return *this;
                    }
                    else
                    {
                        auto grp_vec_view = group_vector_view_type{
                            ids(), *grp_vec_container_it_};
                        grp_vec_view_it_   = grp_vec_view.begin();
                        grp_vec_view_sent_ = grp_vec_view.end();
                    }
                } while (grp_vec_view_it_ == grp_vec_view_sent_);
            }

            return *this;
        }

        constexpr reference operator*() const noexcept
        {
            return *grp_vec_view_it_;
        }

        constexpr const auto& ids() const noexcept
        {
            return grp_vec_view_it_.ids();
        }

        constexpr const auto& ordered_ids() const noexcept
        {
            return grp_vec_view_it_.ordered_ids();
        }

        constexpr auto is_beyond_end() const noexcept
        {
            return grp_vec_container_it_ >= grp_vec_container_sent_;
        }
    };

    struct sentinel
    {
        constexpr sentinel() noexcept = default;

        constexpr auto operator==(const iterator& it) const noexcept
        {
            // use >= over == because the begin can be higher than end on
            // instantiation
            return it.is_beyond_end();
        }

        constexpr auto operator!=(const iterator& it) const noexcept
        {
            return !(*this == it);
        }

        constexpr friend auto operator==(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return sent == it;
        }

        constexpr friend auto operator!=(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return sent != it;
        }
    };

private:
    unordered_ids_type                            ids_;
    matter::iterator_t<std::vector<group_vector>> begin_;
    matter::sentinel_t<std::vector<group_vector>> end_;

public:
    constexpr registry_view(
        const unordered_ids_type&                     ids,
        matter::iterator_t<std::vector<group_vector>> begin,
        matter::sentinel_t<std::vector<group_vector>> end) noexcept
        : ids_{ids}, begin_{begin}, end_{end}
    {}

    constexpr iterator begin() noexcept
    {
        return iterator{ids_, begin_, end_};
    }

    constexpr sentinel end() noexcept
    {
        return sentinel{};
    }

    template<typename Function>
    void for_each(Function f) const
        noexcept(std::is_nothrow_invocable_v<Function, typename TIds::type&...>)
    {
        auto it = begin_ + ids_.size();
        if (it >= end_)
        {
            return;
        }

        matter::for_each(
            it, end_, [ids = ids_, f = std::move(f)](group_vector& grp_vec) {
                matter::group_vector_view view{ids, grp_vec};
                matter::for_each(
                    view.begin(),
                    view.end(),
                    [f = std::move(f)](auto grp_view) {
#pragma omp simd
                        for (std::size_t i = 0; i < grp_view.size(); ++i)
                        {
                            auto comp_view = grp_view[i];
                            comp_view.invoke(std::move(f));
                        }
                    });
            });
    }

    /// erase the entity at index idx of group at it, all components will be
    /// removed with the entity
    void erase(const iterator& it, std::size_t idx) noexcept
    {
        // this is where the to be erased entity lies
        auto any_grp = (*it).underlying_group();
        any_grp.erase(idx);
    }

    /// remove the components from iterator at it
    /// this will move the entity to a different group
    template<typename... Cs>
    std::enable_if_t<(detail::type_in_list_v<Cs, typename TIds::type...> &&
                      ...)>
    detach(const iterator& it, std::size_t idx) noexcept
    {
        any_group current_group     = (*it).underlying_group();
        auto      group_size        = current_group.group_size();
        auto      future_group_size = group_size - sizeof...(Cs);

        // group vector to insert into
        auto& group_vector = *(begin_ + future_group_size);

        auto new_group = group_vector.find_new_group_without(
            const_any_group{current_group},
            matter::unordered_typed_ids<id_type,
                                        decltype(ids_.template get<Cs>())...>{
                ids_.template get<Cs>()...});

        auto ordered_without_ids =
            matter::ordered_typed_ids{ids_.template get<Cs>()...};

        // migrate from our old group to the new group
        matter::for_each(matter::execution::unseq,
                         current_group.begin(),
                         current_group.end(),
                         [&](auto&& er_storage) {
                             if (!ordered_without_ids.contains(er_storage.id()))
                             {
                                 new_group.push_back(er_storage[idx]);
                             }
                         });

        // all sizes must be equal after migration
        assert(new_group.are_sizes_valid());

        // remove the entity from the old array
        current_group.erase(idx);
    }
};

template<typename Id, typename... TIds>
registry_view(const matter::unordered_typed_ids<Id, TIds...>,
              matter::iterator_t<std::vector<group_vector>> begin,
              matter::sentinel_t<std::vector<group_vector>> end)
    ->registry_view<matter::unordered_typed_ids<Id, TIds...>>;
} // namespace matter

#endif
