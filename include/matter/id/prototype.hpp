#ifndef MATTER_ID_PROTOTYPE_HPP
#define MATTER_ID_PROTOTYPE_HPP

#pragma once

#include "matter/id/component_identifier.hpp"
#include "matter/id/id.hpp"
#include "matter/id/typed_id.hpp"

namespace matter
{
namespace prototype
{
struct id
{
    id() noexcept;
    id(const id&) noexcept;
    id& operator=(const id&) noexcept;

    explicit operator bool() const;

    bool operator==(const id&) const;
    bool operator!=(const id&) const;

    bool operator<(const id&) const;
    bool operator>(const id&) const;
    bool operator<=(const id&) const;
    bool operator>=(const id&) const;

    friend void swap(id&, id&);
};

static_assert(matter::is_id_v<matter::prototype::id>);

struct component_identifier
{
    using id_type = matter::signed_id<int>;

    template<typename T>
    bool is_registered() const;

    template<typename T>
    matter::typed_id<id_type, T> register_type();

    template<typename T>
    matter::typed_id<id_type, T> id() const;
};

static_assert(
    matter::is_component_identifier_v<matter::prototype::component_identifier>);
} // namespace prototype
} // namespace matter

#endif
