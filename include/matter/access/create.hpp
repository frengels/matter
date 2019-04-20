#ifndef MATTER_ACCESS_CREATE_HPP
#define MATTER_ACCESS_CREATE_HPP

#pragma once

#include <optional>

#include "matter/access/command/create.hpp"

namespace matter
{
template<typename Registry, typename... Cs>
struct create_meta;

template<typename... Cs>
struct create
{
    template<typename Registry>
    using meta_type = create_meta<Registry, Cs...>;

private:
    std::optional<std::tuple<Cs...>> opt_comps_;

public:
    constexpr create() noexcept = default;

    template<typename... Comps>
    std::enable_if_t<(std::is_constructible_v<Cs, Comps> && ...)>
    operator()(Comps&&... components) noexcept
    {
        opt_comps_ = std::tuple<Cs...>{std::forward<Comps>(components)...};
    }

    template<typename... TupArgs>
    std::enable_if_t<
        (matter::detail::is_constructible_expand_tuple_v<Cs, TupArgs> && ...)>
    operator()(TupArgs&&... tup_args) noexcept
    {
        opt_comps_ = std::tuple<Cs...>{matter::detail::construct_from_tuple<Cs>(
            std::forward<TupArgs>(tup_args))...};
    }

    explicit operator bool() const noexcept
    {
        return bool(opt_comps_);
    }

    template<typename T>
    const T& get() const noexcept
    {
        assert(bool(*this));
        return std::get<T>(*opt_comps_);
    }
};

template<typename Registry, typename... Cs>
struct create_meta
{
    using required_types = void;
    using registry_type  = Registry;

public:
    constexpr create_meta(registry_type& reg) noexcept
    {}

    constexpr bool process_group_vector(const matter::group_vector&) const
        noexcept
    {
        return true;
    }

    constexpr bool process_group(matter::const_any_group) const noexcept
    {
        return true;
    }

    constexpr create<Cs...> make_access(matter::entity_handle) noexcept
    {
        return {};
    }

    constexpr std::optional<matter::command::create<Cs...>>
    create_commands(create<Cs...>&& c) const noexcept
    {
        // create was called
        if (bool(c))
        {
            return {std::move(c.template get<Cs>())...};
        }

        return std::nullopt;
    }
};
} // namespace matter

#endif
