#ifndef MATTER_COMPONENT_GROUP_VIEW_HPP
#define MATTER_COMPONENT_GROUP_VIEW_HPP

#pragma once

#include "matter/component/component_view.hpp"
#include "matter/component/group.hpp"
#include "matter/util/iterator.hpp"
#include "matter/util/traits.hpp"

namespace matter
{
/// \brief View the stores of Cs... in the current group
/// A struct to easily access specific stores in a group. This is meant as a
/// concrete view of the normally abstract group
template<typename Id, typename... Cs>
class group_view {
public:
    using id_type = Id;

    struct iterator
    {
        using difference_type   = int;
        using value_type        = component_view<Cs...>;
        using reference         = component_view<Cs...>;
        using pointer           = void;
        using iterator_category = std::random_access_iterator_tag;

    private:
        template<typename C>
        using storage_iterator_type =
            matter::iterator_t<matter::component_storage_t<C>>;

    private:
        std::tuple<storage_iterator_type<Cs>...> its_;

    public:
        constexpr iterator() = default;

        constexpr iterator(storage_iterator_type<Cs>... its) noexcept
            : its_{its...}
        {}

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return std::get<0>(its_) == std::get<0>(other.its_);
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr iterator& operator+=(int movement) noexcept
        {
            std::apply([movement](auto&&... its) { ((its += movement), ...); },
                       its_);
            return *this;
        }

        constexpr iterator& operator-=(int movement) noexcept
        {
            std::apply([movement](auto&&... its) { ((its -= movement), ...); },
                       its_);
            return *this;
        }

        constexpr iterator operator+(int movement) const noexcept
        {
            iterator tmp{*this};
            tmp += movement;
            return tmp;
        }

        constexpr iterator operator-(int movement) const noexcept
        {
            iterator tmp{*this};
            tmp -= movement;
            return tmp;
        }

        constexpr int operator-(const iterator& other) const noexcept
        {
            return std::get<0>(its_) - std::get<0>(other.its_);
        }

        constexpr iterator& operator++() noexcept
        {
            std::apply([](auto&&... its) { (++its, ...); }, its_);
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            iterator tmp{*this};
            ++(*this);
            return tmp;
        }

        constexpr iterator& operator--() noexcept
        {
            std::apply([](auto&&... its) { (--its, ...); }, its_);
            return *this;
        }

        constexpr iterator operator--(int) noexcept
        {
            iterator tmp{*this};
            --(*this);
            return tmp;
        }

        constexpr component_view<Cs...> operator*() noexcept
        {
            return std::apply(
                [](auto&&... its) { return component_view<Cs...>{*its...}; },
                its_);
        }

        constexpr const component_view<Cs...> operator*() const noexcept
        {
            return std::apply(
                [](auto&&... its) { return component_view<Cs...>{*its...}; },
                its_);
        }

        template<std::size_t N>
        constexpr auto get() const noexcept
        {
            return std::get<N>(its_);
        }

        template<typename C>
        constexpr auto get() const noexcept
        {
            return std::get<storage_iterator_type<C>>(its_);
        }
    };

    struct sentinel
    {
    private:
        template<typename C>
        using storage_sentinel_type =
            matter::sentinel_t<matter::component_storage_t<C>>;

    private:
        storage_sentinel_type<detail::nth_t<0, Cs...>> sent_;

    public:
        sentinel() noexcept = default;

        sentinel(storage_sentinel_type<Cs>... sents) noexcept
            : sent_{std::get<0>(std::make_tuple(sents...))}
        {}

        constexpr auto operator==(const iterator& it) const noexcept
        {
            return sent_ == it.template get<0>();
        }

        constexpr auto operator!=(const iterator& it) const noexcept
        {
            return !(*this == it);
        }

        constexpr auto
        operator==(const std::reverse_iterator<iterator>& it) const noexcept
        {
            return sent_ == it.base().template get<0>();
        }

        constexpr auto
        operator!=(const std::reverse_iterator<iterator>& it) const noexcept
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
            return !(sent == it);
        }

        constexpr friend auto
        operator==(const std::reverse_iterator<iterator>& rit,
                   const sentinel&                        sent) noexcept
        {
            return sent == rit;
        }

        constexpr friend auto
        operator!=(const std::reverse_iterator<iterator>& rit,
                   const sentinel&                        sent) noexcept
        {
            return !(sent == rit);
        }

        constexpr auto operator<(const iterator& it) const noexcept
        {
            return sent_ < it.template get<0>();
        }

        constexpr friend auto operator<(const iterator& it,
                                        const sentinel& sent) noexcept
        {
            return it.template get<0>() < sent.sent_;
        }

        constexpr auto operator-(const iterator& it) const noexcept
        {
            return sent_ - it.template get<0>();
        }

        constexpr friend auto operator-(const iterator& it,
                                        const sentinel& sent) noexcept
        {
            return it.template get<0>() - sent.sent_;
        }
    };

private:
    any_group<id_type> grp_;
    std::tuple<std::reference_wrapper<matter::component_storage_t<Cs>>...>
        stores_;

public:
    constexpr group_view(const unordered_typed_ids<id_type, Cs...>& ids,
                         any_group<id_type> grp) noexcept
        : grp_{grp}, stores_{grp.storage(ids)}
    {}

    constexpr group_view(const group<id_type, Cs...>& grp) noexcept
        : grp_{grp.underlying_group()}, stores_{grp.stores_}
    {}

    constexpr any_group<id_type> underlying_group() const noexcept
    {
        return grp_;
    }

    constexpr auto operator==(const group<id_type, Cs...>& grp) const noexcept
    {
        // compare address instead of comparison the full vector, if the address
        // isn't the same then they're not the same even if the contained
        // elements are the same.
        return ((std::addressof(get<Cs>()) ==
                 std::addressof(grp.template get<Cs>())) &&
                ...);
    }

    constexpr auto operator!=(const group<id_type, Cs...>& grp) const noexcept
    {
        return !(*this == grp);
    }

    friend constexpr auto
    operator==(const group<id_type, Cs...>&      grp,
               const group_view<id_type, Cs...>& grp_view) noexcept
    {
        return grp_view == grp;
    }

    friend constexpr auto
    operator!=(const group<id_type, Cs...>&      grp,
               const group_view<id_type, Cs...>& grp_view) noexcept
    {
        return grp_view != grp;
    }

    constexpr component_view<Cs...> operator[](std::size_t index) noexcept
    {
        return std::apply(
            [index](auto&&... stores) {
                return component_view{stores.get()[index]...};
            },
            stores_);
    }

    template<typename T>
    constexpr matter::component_storage_t<T>& get() noexcept
    {
        static_assert(detail::type_in_list_v<T, Cs...>,
                      "T is not a valid component type for this view.");
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }

    template<typename T>
    constexpr const matter::component_storage_t<T>& get() const noexcept
    {
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }

    template<std::size_t I>
    constexpr matter::component_storage_t<detail::nth_t<I, Cs...>>&
    get() noexcept
    {
        return get<detail::nth_t<I, Cs...>>();
    }

    template<std::size_t I>
    constexpr const matter::component_storage_t<detail::nth_t<I, Cs...>>&
    get() const noexcept
    {
        return get<detail::nth_t<I, Cs...>>();
    }

    constexpr iterator begin() noexcept
    {
        return iterator{get<Cs>().begin()...};
    }

    constexpr auto end() noexcept
    {
        return sentinel{get<Cs>().end()...};
    }

    constexpr std::size_t size() const noexcept
    {
        return get<0>().size();
    }

    constexpr auto empty() const noexcept
    {
        return get<0>().empty();
    }
};

template<typename Id, typename... Cs>
group_view(const unordered_typed_ids<Id, Cs...>& ids,
           any_group<Id> grp) noexcept->group_view<Id, Cs...>;

template<typename Id, typename... Cs>
group_view(const group<Id, Cs...>& grp) noexcept->group_view<Id, Cs...>;

} // namespace matter

#endif
