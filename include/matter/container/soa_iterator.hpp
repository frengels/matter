#pragma once

#include <hera/container/pair.hpp>

#include "matter/concepts/totally_ordered.hpp"
#include "matter/container/soa_proxy.hpp"
#include "matter/iterator/concepts.hpp"
#include "matter/iterator/iter_move.hpp"
#include "matter/ranges/traits.hpp"

namespace matter
{
namespace detail
{
template<typename... Is> // clang-format off
    requires (matter::bidirectional_iterator<Is> && ...)
constexpr void all_bidirectional_iterators(hera::type_list<Is...>) noexcept // clang-format on
{}

template<typename... Is> // clang-format off
    requires (matter::random_access_iterator<Is> && ...)
constexpr void all_random_access_iterators(hera::type_list<Is...>) noexcept // clang-format on
{}

template<typename... Is> // clang-format off
    requires (matter::sized_sentinel_for<Is, Is> || ...)
constexpr void any_sized_sentinel_for(hera::type_list<Is...>) noexcept // clang-format on
{}

template<typename... Is> // clang-format off
    requires (matter::totally_ordered<Is> || ...)
constexpr void any_totally_ordered(hera::type_list<Is...>) noexcept // clang-format on
{}
} // namespace detail

template<typename SoA, typename... Its>
class soa_iterator {
private:
    template<typename SoA_, typename... Its_>
    friend class soa_sentinel;

public:
    using soa_type = SoA;

    using value_type      = matter::soa_proxy<matter::iter_reference_t<Its>...>;
    using reference       = value_type;
    using pointer         = void;
    using difference_type = matter::common_type_t<iter_difference_t<Its>...>;
    using iterator_category = std::forward_iterator_tag;

private:
    [[no_unique_address]] hera::tuple<Its...> iterators_;

public:
    soa_iterator() = default;

    constexpr soa_iterator(hera::type_identity<SoA>, Its... its) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Its...>, Its...>)
        : iterators_{std::move(its)...}
    {}

    constexpr soa_iterator(Its... its) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Its...>, Its...>)
        : iterators_{std::move(its)...}
    {}

    template<typename... UIts>
    constexpr soa_iterator(const soa_iterator<SoA, UIts...>& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Its...>,
                                        const hera::tuple<Its...>&>)
        : iterators_{other.iterators_}
    {}

    template<typename... UIts>
    constexpr soa_iterator(soa_iterator<SoA, UIts...>&& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Its...>,
                                        hera::tuple<UIts...>&&>)
        : iterators_{std::move(other.iterators_)}
    {}

    template<typename... UIts>
    constexpr soa_iterator&
    operator=(const soa_iterator<SoA, UIts...>& other) noexcept(
        std::is_nothrow_assignable_v<hera::tuple<Its...>,
                                     const hera::tuple<UIts...>&>)
    {
        iterators_ = other.iterators_;
        return *this;
    }

    template<typename... UIts>
    constexpr soa_iterator&
    operator=(soa_iterator<SoA, UIts...>&& other) noexcept(
        std::is_nothrow_assignable_v<hera::tuple<Its...>,
                                     hera::tuple<UIts...>&&>)
    {
        iterators_ = std::move(other.iterators_);
        return *this;
    }

    constexpr hera::tuple<Its...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Its...>>)
    {
        return iterators_;
    }

    constexpr bool operator==(const soa_iterator& other) const
        noexcept(noexcept(iterators_.front() == other.iterators_.front()))
    {
        return iterators_.front() == other.iterators_.front();
    }

    constexpr bool operator!=(const soa_iterator& other) const
        noexcept(noexcept(iterators_.front() != other.iterators_.front()))
    {
        return iterators_.front() != other.iterators_.front();
    }

private:
    template<typename F,
             typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_totally_ordered(list);
            } // clang-format on
    constexpr bool generic_compare(const soa_iterator& other, F&& op) const
    {
        auto filtered_its =
            hera::zip_view{iterators_, other.iterators_} |
            hera::views::filter([](auto&& it_pair) {
                using iterator_type =
                    std::remove_cvref_t<decltype(it_pair.front())>;
                return std::bool_constant<
                    matter::totally_ordered<iterator_type>>{};
            });

        auto [it, other_it] = filtered_its.front();
        return std::forward<F>(op)(it, other_it);
    }

public:
    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_totally_ordered(list);
            } // clang-format on
    constexpr bool operator<(const soa_iterator& other) const
    {
        return generic_compare(other, [](const auto& it, const auto& other_it) {
            return it < other_it;
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_totally_ordered(list);
            } // clang-format on
    constexpr bool operator<=(const soa_iterator& other) const
    {
        return generic_compare(other, [](const auto& it, const auto& other_it) {
            return it <= other_it;
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_totally_ordered(list);
            } // clang-format on
    constexpr bool operator>(const soa_iterator& other) const
    {
        return generic_compare(other, [](const auto& it, const auto& other_it) {
            return it > other_it;
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_totally_ordered(list);
            } // clang-format on
    constexpr bool operator>=(const soa_iterator& other) const
    {
        return generic_compare(other, [](const auto& it, const auto& other_it) {
            return it >= other_it;
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::any_sized_sentinel_for(list);
            } // clang-format on
    constexpr difference_type operator-(const soa_iterator& other) const
    {
        auto filtered_its =
            hera::zip_view{iterators_, other.iterators_} |
            hera::views::filter([](auto&& it_pair) {
                using iterator_type =
                    std::remove_cvref_t<decltype(it_pair.front())>;
                return std::bool_constant<
                    matter::sized_sentinel_for<iterator_type, iterator_type>>{};
            });

        auto [it, other_it] = filtered_its.front();
        return it - other_it;
    }

    constexpr soa_iterator& operator++() noexcept
    {
        hera::for_each(iterators_, [](auto& it) { ++it; });
        return *this;
    }

    constexpr soa_iterator operator++(int) noexcept
    {
        auto res = *this;
        ++(*this);
        return res;
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_bidirectional_iterators(list);
            } // clang-format on
    constexpr soa_iterator& operator--() noexcept
    {
        hera::for_each(iterators_, [](auto& it) { --it; });
        return *this;
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_bidirectional_iterators(list);
            } // clang-format on
    constexpr soa_iterator operator--(int) noexcept
    {
        auto res = *this;
        --(*this);
        return res;
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    constexpr soa_iterator& operator+=(difference_type n)
    {
        hera::for_each(iterators_, [&](auto& it) { it += n; });
        return *this;
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    constexpr soa_iterator operator+(difference_type n) const
    {
        return hera::unpack(iterators_, [&](const auto&... its) {
            return soa_iterator{(its + n)...};
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    friend constexpr soa_iterator operator+(difference_type     n,
                                            const soa_iterator& it)
    {
        return hera::unpack(it.iterators_, [&](const auto&... its) {
            return soa_iterator{(n + its)...};
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    constexpr soa_iterator operator-(difference_type n) const
    {
        return hera::unpack(iterators_, [&](const auto&... its) {
            return soa_iterator{(its - n)...};
        });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    constexpr soa_iterator& operator-=(difference_type n)
    {
        hera::for_each(iterators_, [&](auto& it) { it -= n; });
        return *this;
    }

    friend constexpr auto iter_move(const soa_iterator& it)
    {
        return hera::unpack(it.iterators_, [](const auto&... its) {
            return soa_proxy{matter::iter_move(its)...};
        });
    }

    constexpr reference operator*() const
    {
        return hera::unpack(
            iterators_, [](const auto&... its) { return reference{*its...}; });
    }

    template<typename ItList = hera::type_list<Its...>> // clang-format off
        requires
            requires(ItList list)
            {
                detail::all_random_access_iterators(list);
            } // clang-format on
    constexpr reference operator[](difference_type n) const
        noexcept(noexcept(*(*this + n)))
    {
        return *(*this + n);
    }
};
} // namespace matter