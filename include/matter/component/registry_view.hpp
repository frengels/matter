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

        using group_view_type = matter::group_view<typename TIds::type...>;
        using group_view_iterator_type = matter::iterator_t<group_view_type>;
        using group_view_sentinel_type = matter::sentinel_t<group_view_type>;

        using value_type = matter::component_view<typename TIds::type...>;
        using reference  = value_type;
        using pointer    = void;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = typename std::iterator_traits<
            matter::iterator_t<group_view_type>>::difference_type;

    private:
        unordered_ids_type ids_;

        group_vector_container_iterator_type grp_vec_container_it_;
        group_vector_container_sentinel_type grp_vec_container_sent_;

        group_vector_view_iterator_type grp_vec_view_it_;
        group_vector_view_sentinel_type grp_vec_view_sent_;

        group_view_iterator_type grp_view_it_;
        group_view_sentinel_type grp_view_sent_;

    public:
        constexpr iterator(
            const unordered_ids_type&            ids,
            group_vector_container_iterator_type begin_range,
            group_vector_container_sentinel_type end_range) noexcept
            : ids_{ids}, grp_vec_container_it_{begin_range + std::size(ids)},
              grp_vec_container_sent_{end_range}, grp_vec_view_it_{},
              grp_vec_view_sent_{}, grp_view_it_{}, grp_view_sent_{}
        {
            grp_vec_container_it_   = begin_range + std::size(ids);
            grp_vec_container_sent_ = end_range;

            for (; grp_vec_container_it_ < grp_vec_container_sent_;
                 ++grp_vec_container_it_)
            {
                auto grp_vec_view =
                    group_vector_view_type{ids_, *grp_vec_container_it_};

                grp_vec_view_it_   = grp_vec_view.begin();
                grp_vec_view_sent_ = grp_vec_view.end();
                for (; grp_vec_view_it_ != grp_vec_view_sent_;
                     ++grp_vec_view_it_)
                {
                    auto grp_view = *grp_vec_view_it_;

                    grp_view_it_   = grp_view.begin();
                    grp_view_sent_ = grp_view.end();
                    for (; grp_view_it_ != grp_view_sent_; ++grp_view_it_)
                    {
                        // simply iterate until we get to our first valid
                        // component_view, return afterwards as we're now in a
                        // valid state
                        return;
                    }
                }
            }
        }

        constexpr iterator& operator++() noexcept
        {
            // if we hit the end of our current group
            // we need to find our next group to view
            if (++grp_view_it_ == grp_view_sent_)
            {
                do
                {
                    // if our current group_vector is at the end of valid groups
                    // of this size we need to go to the next group_vector
                    if (++grp_vec_view_it_ == grp_vec_view_sent_)
                    {
                        do
                        {
                            // if our next container iterator is larger than the
                            // sentinel it means we've exhausted our group
                            // vectors and are now officialy in the end state
                            if (++grp_vec_container_it_ ==
                                grp_vec_container_sent_)
                            {
                                // end state, do nothing
                                return *this;
                            }
                            else
                            {
                                auto grp_vec_view = group_vector_view_type{
                                    ids_, *grp_vec_container_it_};
                                grp_vec_view_it_   = grp_vec_view.begin();
                                grp_vec_view_sent_ = grp_vec_view.end();
                            }
                            // keep incrementing if there are no valid groups of
                            // the specified size found
                        } while (grp_vec_view_it_ == grp_vec_view_sent_);
                    }
                    else
                    {
                        auto grp_view  = *grp_vec_view_it_;
                        grp_view_it_   = grp_view.begin();
                        grp_view_sent_ = grp_view.end();
                    }

                    // skip over any empty groups
                } while (grp_view_it_ == grp_view_sent_);
            }

            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto it = *this;
            ++(*this);
            return it;
        }

        constexpr reference operator*() const noexcept
        {
            return *grp_view_it_;
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
            return it.is_beyond_end();
        }

        constexpr auto operator!=(const iterator& it) const noexcept
        {
            return !(*this == it);
        }

        constexpr friend auto operator==(const iterator& it,
                                         const sentinel&) noexcept
        {
            return it.is_beyond_end();
        }

        constexpr friend auto operator!=(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return !(sent == it);
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

    constexpr sentinel end() const noexcept
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
                matter::for_each(view.begin(),
                                 view.end(),
                                 [f = std::move(f)](auto grp_view) {
                                     matter::for_each(
                                         grp_view.begin(),
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
              matter::iterator_t<std::vector<group_vector>> begin,
              matter::sentinel_t<std::vector<group_vector>> end)
    ->registry_view<matter::unordered_typed_ids<Id, TIds...>>;
} // namespace matter

#endif
