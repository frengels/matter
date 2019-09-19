#pragma once

#include <utility>

#include <hera/container/tuple.hpp>

#include "matter/concepts/invocable.hpp"
#include "matter/utility/common_reference.hpp"

namespace matter
{
// a proxy stores and returns the Ts... in the signature
// This class is purely meant to be destructured and is basically just a wrapper
// around hera::tuple<Ts...>.
template<typename... Ts>
class soa_proxy {
private:
    hera::tuple<Ts...> values_;

public:
    constexpr soa_proxy(Ts... vals) noexcept
        : values_{std::forward<Ts>(vals)...}
    {}

    template<typename... Us>
    constexpr soa_proxy(const matter::soa_proxy<Us...>& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Ts...>,
                                        const hera::tuple<Us...>&>)
        : values_{other.values_}
    {}

    template<typename... Us>
    constexpr soa_proxy(matter::soa_proxy<Us...>&& other) noexcept(
        std::is_nothrow_constructible_v<hera::tuple<Ts...>,
                                        hera::tuple<Us...>&&>)
        : values_{std::move(other.values_)}
    {}

    template<invocable<Ts...> F>
    constexpr decltype(auto)
    unpack(F&& fn) const& noexcept(std::is_nothrow_invocable_v<F, Ts...>)
    {
        return hera::unpack(values_, [&](auto&&... values) {
            return std::invoke(std::forward<F>(fn),
                               std::forward<decltype(values)>(values)...);
        });
    }

    template<invocable<Ts...> F>
    constexpr decltype(auto)
    unpack(F&& fn) const&& noexcept(std::is_nothrow_invocable_v<F, Ts...>)
    {
        return hera::unpack(std::move(values_), [&](auto&&... values) {
            return std::invoke(std::forward<F>(fn),
                               std::forward<decltype(values)>(values)...);
        });
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(soa_proxy& prox) noexcept
    {
        return prox.values_[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(const soa_proxy& prox) noexcept
    {
        return prox.values_[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(soa_proxy&& prox) noexcept
    {
        return std::move(
            prox.values_)[std::integral_constant<std::size_t, I>{}];
    }

    template<std::size_t I> // clang-format off
        requires (I < sizeof...(Ts)) // clang-format on
        friend constexpr decltype(auto) get(const soa_proxy&& prox) noexcept
    {
        return std::move(
            prox.values_)[std::integral_constant<std::size_t, I>{}];
    }
};

// values should not get moved into the proxy, they will be forwarded through
// it. Therefore not invoking any redundant move constructors or moving from
// when not intended.
template<typename... Ts>
soa_proxy(Ts&&...)->soa_proxy<Ts&&...>;

template<typename... Ts,
         typename... Us,
         template<typename>
         typename TQual,
         template<typename>
         typename UQual>
struct basic_common_reference<matter::soa_proxy<Ts...>,
                              matter::soa_proxy<Us...>,
                              TQual,
                              UQual>
{
    using type = matter::soa_proxy<common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace matter

namespace std
{
// proxy
template<typename... Ts>
struct tuple_size<matter::soa_proxy<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};

template<std::size_t I, typename... Ts> // clang-format off
    requires (I < sizeof...(Ts))
struct tuple_element<I, matter::soa_proxy<Ts...>> // clang-format on
{
    using type = std::tuple_element_t<I, std::tuple<Ts...>>;
};
} // namespace std