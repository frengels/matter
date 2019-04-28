#ifndef MATTER_COMPONENT_GROUP_HPP
#define MATTER_COMPONENT_GROUP_HPP

#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <iterator>

#include "matter/component/any_group.hpp"
#include "matter/component/component_view.hpp"
#include "matter/component/insert_buffer.hpp"
#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/util/container.hpp"
#include "matter/util/id_erased.hpp"

namespace matter
{
template<typename... Cs>
struct group
{
public:
    struct iterator
    {
        using value_type        = matter::component_view<Cs...>;
        using reference         = matter::component_view<Cs...>;
        using pointer           = void;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        std::tuple<typename matter::component_storage_t<Cs>::iterator...> its_;

    public:
        constexpr iterator(
            typename matter::component_storage_t<Cs>::iterator... its)
            : its_{its...}
        {}

        constexpr auto operator==(const iterator& other) const noexcept
        {
            return first_it() == other.first_it();
        }

        constexpr auto operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr iterator& operator++() noexcept
        {
            std::apply([](auto&... its) { (++its, ...); });
            return *this;
        }

        constexpr iterator& operator--() noexcept
        {
            std::apply([](auto&... its) { (--its, ...); });
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto it = *this;
            ++(*this);
            return it;
        }

        constexpr iterator operator--(int) noexcept
        {
            auto it = *this;
            --(*this);
            return it;
        }

        constexpr iterator& operator+=(difference_type movement) noexcept
        {
            std::apply(
                [movement](auto&... its) { (its.operator+=(movement), ...); },
                its_);

            return *this;
        }

        constexpr iterator& operator-=(difference_type movement) noexcept
        {
            std::apply(
                [movement](auto&... its) { (its.operator-=(movement), ...); },
                its_);

            return *this;
        }

        constexpr iterator operator+(difference_type movement) const noexcept
        {
            auto it = *this;
            it += movement;
            return it;
        }

        constexpr iterator operator-(difference_type movement) const noexcept
        {
            auto it = *this;
            it -= movement;
            return it;
        }

        constexpr reference operator*() noexcept
        {
            return std::apply(
                [](auto&&... its) { return matter::component_view{*its...}; },
                its_);
        }

        constexpr const reference operator*() const noexcept
        {
            return std::apply(
                [](auto&&... its) { return matter::component_view{*its...}; },
                its_);
        }

        template<typename C>
        std::enable_if_t<detail::type_in_list_v<C, Cs...>,
                         typename matter::component_storage_t<C>::iterator>
        get() const noexcept
        {
            return std::get<typename matter::component_storage_t<C>::iterator>(
                its_);
        }

        template<std::size_t N>
        auto get() const noexcept
        {
            return std::get<N>(its_);
        }

    private:
        constexpr auto first_it() const noexcept
        {
            return std::get<0>(its_);
        }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    template<typename... Ts>
    friend class group_view;

    template<typename... Ts>
    friend struct group;

private:
    matter::erased_storage* underlying_storage_;
    std::tuple<std::reference_wrapper<matter::component_storage_t<Cs>>...>
        stores_;

public:
    template<typename Id, typename... TIds>
    constexpr group(const matter::unordered_typed_ids<Id, TIds...>& unordered,
                    any_group& grp) noexcept
        : underlying_storage_{grp.data()}, stores_{grp.storage(unordered)}
    {
        // require exact match and not just contains being satisfied
        assert(grp == matter::ordered_typed_ids{unordered});
    }

    template<typename... OtherCs>
    constexpr group(const matter::group<OtherCs...>& other) noexcept
        : underlying_storage_{other.underlying_storage_},
          stores_{
              std::get<std::reference_wrapper<matter::component_storage_t<Cs>>>(
                  other.stores_)...}
    {
        static_assert((detail::type_in_list_v<OtherCs, Cs...> && ...),
                      "Incompatible components, cannot construct");
    }

    constexpr any_group underlying_group() const noexcept
    {
        return any_group{underlying_storage_, group_size()};
    }

    constexpr auto operator==(const group&) const noexcept
    {
        // there can only ever be one group of a certain type in existence
        return true;
    }

    constexpr auto operator!=(const group& other) const noexcept
    {
        return !(*this == other);
    }

    template<typename T>
    constexpr auto contains() const noexcept
    {
        return detail::type_in_list_v<T, Cs...>;
    }

    template<typename TId>
    constexpr auto contains(const TId&) const noexcept
    {
        return contains<typename TId::type>();
    }

    template<typename Id, typename... TIds>
    constexpr auto
    contains(const matter::unordered_typed_ids<Id, TIds...>&) const noexcept
    {
        return (contains<typename TIds::type>() && ...);
    }

    template<typename Id, typename... TIds>
    constexpr auto contains(const matter::ordered_typed_ids<Id, TIds...>&) const
        noexcept
    {
        return (contains<typename TIds::type>() && ...);
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

    constexpr iterator erase(iterator pos) noexcept
    {
        std::apply(
            [&](auto... stores) {
                (stores.erase(pos.template get<Cs>()), ...);
            },
            stores_);
    }

    template<std::size_t... Is, typename TCArgs>
    constexpr component_view<Cs...>
    _emplace_back_impl(std::index_sequence<Is...>, TCArgs&& cargs)
    {
        std::apply(
            [&](auto... stores) {
                (matter::emplace_back_tuple(
                     stores.get(), std::get<Is>(std::forward<TCArgs>(cargs))),
                 ...);
            },
            stores_);

        return back();
    }

    template<typename... CArgs>
    constexpr component_view<Cs...> emplace_back(CArgs&&... cargs) noexcept(
        (detail::is_nothrow_constructible_expand_tuple_v<Cs, CArgs> && ...))
    {
        return _emplace_back_impl(
            std::index_sequence_for<Cs...>{},
            std::make_tuple(std::forward<CArgs>(cargs)...));
    }

    constexpr component_view<Cs...> push_back(const Cs&... comps)
    {
        return emplace_back(std::forward_as_tuple(comps)...);
    }

    constexpr component_view<Cs...> push_back(Cs&&... comps)
    {
        return emplace_back(std::tuple{std::move(comps)}...);
    }

    template<typename Id, typename... TIds>
    constexpr std::enable_if_t<
        (detail::type_in_list_v<typename TIds::type, Cs...> && ...),
        iterator>
    insert_back(
        const matter::insert_buffer<matter::unordered_typed_ids<Id, TIds...>>&
            buffer) noexcept((std::
                                  is_nothrow_constructible_v<
                                      Cs,
                                      decltype(*buffer.template begin<Cs>())> &&
                              ...))
    {
        return iterator{get<Cs>().insert(
            get<Cs>().end(),
            std::make_move_iterator(buffer.template begin<Cs>()),
            std::make_move_iterator(buffer.template end<Cs>()))...};
    }

    constexpr component_view<Cs...> back() noexcept
    {
        return component_view{get<Cs>().back()...};
    }

    static constexpr std::size_t group_size() noexcept
    {
        return sizeof...(Cs);
    }

    constexpr std::size_t size() const noexcept
    {
        return std::get<0>(stores_).get().size();
    }

    constexpr auto empty() const noexcept
    {
        return std::get<0>(stores_).get().empty();
    }

    constexpr matter::component_view<Cs...> operator[](std::size_t i) noexcept
    {
        return {get<Cs>()[i]...};
    }

private:
    template<typename T>
    constexpr auto& get() noexcept
    {
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }

    template<typename T>
    constexpr const auto& get() const noexcept
    {
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }
};

template<typename Id, typename... TIds>
group(const matter::unordered_typed_ids<Id, TIds...>&,
      any_group&) noexcept->group<typename TIds::type...>;

} // namespace matter

#endif
