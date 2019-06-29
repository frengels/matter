#ifndef MATTER_QUERY_COMPONENT_QUERY_DESCRIPTION_HPP
#define MATTER_QUERY_COMPONENT_QUERY_DESCRIPTION_HPP

#pragma once

#include "matter/id/component_identifier.hpp"
#include "matter/id/id.hpp"
#include "matter/query/access.hpp"
#include "matter/query/presence.hpp"
#include "matter/query/runtime.hpp"
#include "matter/query/typed_access.hpp"

namespace matter
{
template<typename Id>
class component_query_description {
    static_assert(matter::is_id_v<Id>);

public:
    using id_type = Id;

private:
    const id_type          id_;
    const matter::access   access_;
    const matter::presence pres_;

public:
    constexpr component_query_description(const id_type&   id,
                                          matter::access   acc,
                                          matter::presence pres) noexcept
        : id_{id}, access_{acc}, pres_{pres}
    {}

    template<typename Identifier,
             typename T,
             typename Access,
             typename Presence>
    constexpr component_query_description(
        const Identifier& ident,
        boost::hana::basic_type<
            matter::typed_access<T, Access, Presence>>) noexcept
        : id_{ident.template id<T>()}, access_{Access::access_enum()},
          pres_{Presence::presence_enum()}
    {
        static_assert(matter::is_component_identifier_v<Identifier>);
    }

    constexpr const id_type& id() const noexcept
    {
        return id_;
    }

    constexpr matter::presence presence() const noexcept
    {
        return pres_;
    }

    constexpr matter::access access() const noexcept
    {
        return access_;
    }

    constexpr bool is_read() const noexcept
    {
        return access() == matter::access::read;
    }

    constexpr bool is_write() const noexcept
    {
        return access() == matter::access::write;
    }

    constexpr bool is_inaccessible() const noexcept
    {
        return access() == matter::access::inaccessible;
    }

    constexpr bool is_required() const noexcept
    {
        return presence() == matter::presence::require;
    }

    constexpr bool is_optional() const noexcept
    {
        return presence() == matter::presence::optional;
    }

    constexpr bool is_excluded() const noexcept
    {
        return presence() == matter::presence::exclude;
    }

    // used to verify whether 2 queries can coexist concurrently, this member
    // function gets used for both the runtime calculation of concurrency and at
    // compile time.
    constexpr bool can_access_concurrent(
        const component_query_description<id_type>& other) const noexcept
    {
        // if we require no access then we can always run concurrently
        if (is_inaccessible() || other.is_inaccessible())
        {
            return true;
        }

        if (id() != other.id()) // id is different, don't care
        {
            return true;
        }

        // if both have the exclude presence specified then no mutation can take
        // place anyway
        if (is_excluded() || other.is_excluded())
        {
            return true;
        }

        // either required or optional in here
        // with required or optional we now need to check for write/read
        // access reads can occur concurrently but any write cannot
        if (is_write() || other.is_write())
        {
            return false;
        }

        // both are only reading, safe concurrency
        return true;
    }
};

template<typename Identifier, typename T, typename Access, typename Presence>
component_query_description(
    const Identifier& ident,
    boost::hana::basic_type<matter::typed_access<T, Access, Presence>>)
    ->component_query_description<typename Identifier::id_type>;
} // namespace matter

#endif
