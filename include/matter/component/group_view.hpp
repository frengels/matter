#ifndef MATTER_COMPONENT_GROUP_VIEW_HPP
#define MATTER_COMPONENT_GROUP_VIEW_HPP

#pragma once

#include "matter/component/component_view.hpp"
#include "matter/component/group.hpp"
#include "matter/util/iterator.hpp"

namespace matter
{
/// \brief View the stores of Cs... in the current group
/// A struct to easily access specific stores in a group. This is meant as a
/// concrete view of the normally abstract group
template<typename... Cs>
struct group_view
{
public:
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
            matter::iterator_t<typename matter::component_storage_t<C>>;

    private:
        std::tuple<storage_iterator_type<Cs>...> its_;

    public:
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
            std::apply([movement](auto&... its) { ((its += movement), ...); },
                       its_);
            return *this;
        }

        constexpr iterator& operator-=(int movement) noexcept
        {
            std::apply([movement](auto&... its) { ((its -= movement), ...); },
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
            std::apply([](auto&... its) { (++its, ...); }, its_);
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
            std::apply([](auto&... its) { (--its, ...); }, its_);
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
                [](auto... its) { return component_view<Cs...>{*its...}; },
                its_);
        }

        constexpr const component_view<Cs...> operator*() const noexcept
        {
            return std::apply(
                [](auto... its) { return component_view<Cs...>{*its...}; },
                its_);
        }
    };

private:
    std::tuple<std::reference_wrapper<matter::component_storage_t<Cs>>...>
        stores_;

public:
    template<typename Id, typename... TIds>
    constexpr group_view(const unordered_typed_ids<Id, TIds...>& ids,
                         any_group&                              grp) noexcept
        : stores_{grp.storage(ids)}
    {}

    constexpr group_view(const group<Cs...>& grp) noexcept
        : stores_{grp.stores_}
    {}

    template<typename Id>
    constexpr auto operator==(const group<Id, Cs...>& grp) const noexcept
    {
        // compare address instead of comparison the full vector, if the address
        // isn't the same then they're not the same even if the contained
        // elements are the same.
        return ((std::addressof(get<Cs>()) ==
                 std::addressof(grp.template get<Cs>())) &&
                ...);
    }

    template<typename Id>
    constexpr auto operator!=(const group<Id, Cs...>& grp) const noexcept
    {
        return !(*this == grp);
    }

    template<typename Id>
    friend constexpr auto operator==(const group<Id, Cs...>&  grp,
                                     const group_view<Cs...>& grp_view) noexcept
    {
        return grp_view == grp;
    }

    template<typename Id>
    friend constexpr auto operator!=(const group<Id, Cs...>&  grp,
                                     const group_view<Cs...>& grp_view) noexcept
    {
        return grp_view != grp;
    }

    constexpr component_view<Cs...> operator[](std::size_t index) noexcept
    {
        return std::apply([index](auto&... stores) {
            return component_view{stores[index]...};
        });
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
        return std::apply(
            [](auto... stores) { return iterator{stores.get().begin()...}; },
            stores_);
    }

    constexpr iterator end() noexcept
    {
        return std::apply(
            [](auto... stores) { return iterator{stores.get().end()...}; },
            stores_);
    }

    constexpr auto size() const noexcept
    {
        return std::get<0>(stores_).get().size();
    }

    constexpr auto empty() const noexcept
    {
        return std::get<0>(stores_).get().empty();
    }
};

template<typename Id, typename... TIds>
group_view(const unordered_typed_ids<Id, TIds...>& ids,
           any_group& grp) noexcept->group_view<typename TIds::type...>;

template<typename... Cs>
group_view(const group<Cs...>& grp) noexcept->group_view<Cs...>;

} // namespace matter

#endif
