#ifndef MATTER_COMPONENT_COMPONENT_VIEW_HPP
#define MATTER_COMPONENT_COMPONENT_VIEW_HPP

#pragma once

#include "matter/component/traits.hpp"

namespace matter
{
template<typename... Cs>
struct component_view
{
    static_assert((matter::is_component_v<Cs> && ...),
                  "One of the Cs... is not a valid component");

private:
    std::tuple<std::reference_wrapper<Cs>...> components_;

public:
    constexpr component_view(Cs&... comps) noexcept : components_{comps...}
    {}

    template<typename T>
    constexpr T& get() noexcept
    {
        static_assert(detail::type_in_list_v<T, Cs...>,
                      "T is not contained within this component_view.");
        return std::get<std::reference_wrapper<T>>(components_).get();
    }

    template<typename T>
    constexpr const T& get() const noexcept
    {
        static_assert(detail::type_in_list_v<T, Cs...>,
                      "T is not contained within this component_view.");
        return std::get<std::reference_wrapper<T>>(components_).get();
    }

    template<std::size_t I>
    constexpr auto& get() noexcept
    {
        return std::get<I>(components_).get();
    }

    template<std::size_t I>
    constexpr const auto& get() const noexcept
    {
        return std::get<I>(components_).get();
    }

    template<typename F>
    constexpr std::enable_if_t<std::is_invocable_v<F, Cs&...>,
                               std::invoke_result_t<F, Cs&...>>
    invoke(F fn) noexcept(std::is_nothrow_invocable_v<F, Cs&...>)
    {
        return std::apply(
            [f = std::move(fn)](auto... ref_wrap_comps) {
                return f(ref_wrap_comps.get()...);
            },
            components_);
    }

    template<typename F>
    constexpr std::enable_if_t<std::is_invocable_v<F, const Cs&...>,
                               std::invoke_result_t<F, const Cs&...>>
    invoke(F fn) const noexcept(std::is_nothrow_invocable_v<F, const Cs&...>)
    {
        return std::apply(
            [f = std::move(fn)](auto... ref_wrap_comps) {
                return f(ref_wrap_comps.get()...);
            },
            components_);
    }
};

template<typename... Cs>
component_view(Cs&... comps) noexcept
    ->component_view<std::remove_reference_t<std::remove_cv_t<Cs>>...>;

} // namespace matter

namespace std
{
template<typename... Cs>
struct tuple_size<matter::component_view<Cs...>>
    : std::integral_constant<std::size_t, sizeof...(Cs)>
{};

template<std::size_t N, typename... Cs>
struct tuple_element<N, matter::component_view<Cs...>>
{
    using type = decltype(
        std::declval<matter::component_view<Cs...>>().template get<N>());
};
} // namespace std

#endif
