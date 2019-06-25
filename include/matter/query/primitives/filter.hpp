#ifndef MATTER_QUERY_COMPONENT_FILTER_HPP
#define MATTER_QUERY_COMPONENT_FILTER_HPP

#pragma once

#include "matter/component/any_group.hpp"
#include "matter/query/component_query_description.hpp"
#include "matter/query/typed_access.hpp"

namespace matter
{
// filter the group for the specified access.
// this function will return a result of the form
// std::optional<std::tuple<component_storage<T>, ...>>. Where nullopt means the
// filtering failed and the group does not qualify.
template<typename Identifier,
         typename... Ts,
         typename... Access,
         typename... Presence>
constexpr auto filter_group(
    matter::any_group<typename Identifier::id_type> grp,
    const Identifier&                               ident,
    boost::hana::basic_type<
        matter::typed_access<Ts, Access, Presence>>... access_types) noexcept
{
    auto process_storage = [&](auto tid, auto access_type) {
        using storage_modifier_type =
            typename decltype(access_type)::type::access_type::storage_modifier;
        using storage_filter_type =
            typename decltype(access_type)::type::presence_type::storage_filter;

        auto* store = grp.maybe_storage(tid);

        auto modifier = storage_modifier_type{};
        auto filter   = storage_filter_type{};

        auto modified_store = modifier(store);
        return filter(modified_store);
    };

    auto results = std::tuple<decltype(
        process_storage(ident.template id<Ts>(), access_types))...>{};

    auto shortcircuit = [&](auto tid, auto access_type) {
        auto filter_res = process_storage(tid, access_type);

        bool result                             = bool(filter_res);
        std::get<decltype(filter_res)>(results) = std::move(filter_res);

        return result;
    };

    // this will terminate prematurely on false result.
    // because if one filter doesn't pass then we can throw away the result
    // either way
    auto success = (shortcircuit(ident.template id<Ts>(), access_types) && ...);

    auto dereference_results = [](auto&& result) {
        return std::apply(
            [](auto&&... results) {
                return std::optional{std::make_tuple(*std::move(results)...)};
            },
            std::move(result));
    };

    using optional_type = decltype(dereference_results(std::move(results)));

    if (success)
    {
        return dereference_results(std::move(results));
    }
    else
    {
        return optional_type{std::nullopt};
    }
}
// TODO: filtering for runtime component access
} // namespace matter

#endif
