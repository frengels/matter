#ifndef MATTER_ID_ID_CACHE_HPP
#define MATTER_ID_ID_CACHE_HPP

#pragma once

#include <array>

#include <boost/hana/tuple.hpp>

#include "matter/id/component_identifier.hpp"
#include "matter/id/id.hpp"
#include "matter/id/typed_id.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
// A place to store needed ids at compile time. The inner workings of this class
// are nearly identical to unordered_typed_ids, the difference being in the
// intent of the classes. This class is meant to specifically fulfill the
// ComponentIdentifier concept.
template<typename Id, typename... Cs>
class id_cache {
    static_assert(matter::is_id_v<Id>);

public:
    using id_type = Id;

private:
    std::tuple<matter::typed_id<id_type, Cs>...> ids_;

public:
    constexpr id_cache(const matter::typed_id<id_type, Cs>&... ids) noexcept
        : ids_{ids...}
    {}

    template<typename Identifier>
    constexpr id_cache(const Identifier& ident)
        : id_cache{ident.template id<Cs>()...}
    {
        static_assert(matter::is_component_identifier_v<Identifier>);
    }

    template<typename Identifier>
    constexpr id_cache(const Identifier& ident, boost::hana::basic_type<Cs>...)
        : id_cache{ident.template id<Cs>()...}
    {
        static_assert(matter::is_component_identifier_v<Identifier>);
    }

    template<typename T>
    constexpr std::enable_if_t<matter::detail::type_in_list_v<T, Cs...>,
                               matter::typed_id<id_type, T>>
    id() const noexcept
    {
        return std::get<matter::typed_id<id_type, T>>(ids_);
    }

    template<typename T>
    constexpr bool contains() const noexcept
    {
        return matter::detail::type_in_list_v<T, Cs...>;
    }
};
template<typename Identifier, typename... Cs>
id_cache(const Identifier& ident, boost::hana::basic_type<Cs>...)
    ->id_cache<typename Identifier::id_type, Cs...>;
} // namespace matter

#endif
