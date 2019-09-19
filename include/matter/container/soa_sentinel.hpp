#pragma once

#include <hera/view/filter.hpp>
#include <hera/view/zip.hpp>

#include "matter/iterator/concepts.hpp"
#include "matter/iterator/traits.hpp"

namespace matter
{
namespace detail
{
template<typename... Sents, typename... Its> // clang-format off
    requires (sized_sentinel_for<Sents, Its> || ...)
constexpr void any_sized_sentinel_for(hera::type_list<Sents...>, hera::type_list<Its...>) noexcept // clang-format on
{}

template<typename... Sents, typename... Its> // clang-format off
    requires (sentinel_for<Sents, Its> && ...)
constexpr void all_sentinel_for(hera::type_list<Sents...>, hera::type_list<Its...>) noexcept // clang-format on
{}
} // namespace detail

template<typename SoA, typename... Sents>
class soa_sentinel {
public:
    using soa_type = SoA;

private:
    [[no_unique_address]] hera::tuple<Sents...> sentinels_;

public:
    soa_sentinel() = default;

    template<typename... USents>
    constexpr soa_sentinel(
        hera::type_identity<SoA>,
        USents&&... sents) noexcept(std::
                                        is_nothrow_constructible_v<
                                            hera::tuple<Sents...>,
                                            USents...>)
        : sentinels_{std::forward<USents>(sents)...}
    {}

    template<typename... USents>
    constexpr soa_sentinel(USents&&... sents) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Sents...>, USents...>)
        : sentinels_{std::forward<USents>(sents)...}
    {}

    template<typename... USents>
    constexpr soa_sentinel(const soa_sentinel<SoA, USents...>& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Sents...>,
                                        const hera::tuple<USents...>>)
        : sentinels_{other.sentinels_}
    {}

    template<typename... USents>
    constexpr soa_sentinel(soa_sentinel<SoA, USents...>&& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Sents...>,
                                        hera::tuple<USents...>&&>)
        : sentinels_{std::move(other.sentinels_)}
    {}

    template<typename... USents>
    constexpr soa_sentinel&
    operator=(const soa_sentinel<SoA, USents...>& other) noexcept(
        std::is_nothrow_assignable_v<hera::tuple<Sents...>,
                                     const hera::tuple<USents...>&>)
    {
        sentinels_ = other.sentinels_;
        return *this;
    }

    template<typename... USents>
    constexpr soa_sentinel&
    operator=(soa_sentinel<SoA, USents...>&& other) noexcept(
        std::is_nothrow_assignable_v<hera::tuple<Sents...>,
                                     hera::tuple<USents...>&&>)
    {
        sentinels_ = std::move(other.sentinels_);
    }

    constexpr hera::tuple<Sents...> base() const
        noexcept(std::is_nothrow_copy_constructible_v<hera::tuple<Sents...>>)
    {
        return sentinels_;
    }

    constexpr bool operator==(const soa_sentinel& other) const
        noexcept(noexcept(sentinels_.first() == other.sentinels_.first()))
    {
        return sentinels_.first() == other.sentinels_.first();
    }

    constexpr bool operator!=(const soa_sentinel& other) const
        noexcept(noexcept(sentinels_.first() != other.sentinels_.first()))
    {
        return sentinels_.first() != other.sentinels_.first();
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::all_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    constexpr bool operator==(const soa_iterator<SoA, Its...>& it) noexcept(
        noexcept(sentinels_.first() == it.iterators_.first()))
    {
        return sentinels_.first() == it.iterators_.first();
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::all_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    constexpr bool operator!=(const soa_iterator<SoA, Its...>& it) noexcept(
        noexcept(sentinels_.first() != it.iterators_.first()))
    {
        return sentinels_.first() != it.iterators_.first();
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::all_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    friend constexpr bool operator==(
        const soa_iterator<SoA, Its...>& it,
        const soa_sentinel& sent) noexcept(noexcept(it.iterators_.first() ==
                                                    sent.sentinels_.first()))
    {
        return it.iterators_.first() == sent.sentinels_.first();
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::all_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    friend constexpr bool operator!=(
        const soa_iterator<SoA, Its...>& it,
        const soa_sentinel& sent) noexcept(noexcept(it.iterators_.first() !=
                                                    sent.sentinels_.first()))
    {
        return it.iterators_.first() != sent.sentinels_.first();
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::any_sized_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    constexpr iter_difference_t<soa_iterator<SoA, Its...>>
    operator-(const soa_iterator<SoA, Its...>& it) const
    {
        auto filtered_sents =
            hera::zip_view{sentinels_, it.iterators_} |
            hera::views::filter([](const auto& sent_it) {
                using sentinel_type =
                    std::remove_cvref_t<decltype(sent_it.front())>;
                using iterator_type =
                    std::remove_cvref_t<decltype(sent_it.back())>;

                return std::bool_constant<
                    matter::sized_sentinel_for<sentinel_type, iterator_type>>{};
            });

        auto [base_sent, base_it] = filtered_sents.front();
        return base_sent - base_it;
    }

    template<typename... Its> // clang-format off
        requires sizeof...(Its) == sizeof...(Sents) &&
            requires(hera::type_list<Sents...> sent_tl, hera::type_list<Its...> it_tl)
            {
                detail::any_sized_sentinel_for(sent_tl, it_tl);
            } // clang-format on
    friend constexpr iter_difference_t<soa_iterator<SoA, Its...>>
    operator-(const soa_iterator<SoA, Its...>& it, const soa_sentinel& sent)
    {
        auto filtered_sents =
            hera::zip_view{sent.sentinels_, it.iterators_} |
            hera::views::filter([](const auto& sent_it) {
                using sentinel_type =
                    std::remove_cvref_t<decltype(sent_it.front())>;
                using iterator_type =
                    std::remove_cvref_t<decltype(sent_it.back())>;

                return std::bool_constant<
                    matter::sized_sentinel_for<sentinel_type, iterator_type>>{};
            });

        auto [base_sent, base_it] = filtered_sents.front();
        return base_it - base_sent;
    }
};

template<typename SoA, typename... USents>
soa_sentinel(hera::type_identity<SoA>, USents&&...)
    ->soa_sentinel<SoA, std::decay_t<USents>...>;
} // namespace matter