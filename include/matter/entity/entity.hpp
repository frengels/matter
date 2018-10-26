#ifndef MATTER_ENTITY_HPP
#define MATTER_ENTITY_HPP

#include <type_traits>

#pragma once

namespace matter
{
/*! \brief The standard entity type to use
 *
 */
template<typename Tid, typename Tversion>
struct entity
{
    static_assert(std::is_integral<Tid>::value, "Tid must be an integral type");
    static_assert(std::is_integral<Tversion>::value,
                  "Tversion must be an integral type");

public:
    using id_type      = Tid;
    using version_type = Tversion;

    static constexpr id_type invalid_id = 0;

private:
    id_type m_id;
    version_type m_version;

public:
    constexpr id_type id() const noexcept
    {
        return m_id;
    }

    constexpr version_type version() const noexcept
    {
        return m_version;
    }

    constexpr bool operator==(const entity& other) const noexcept
    {
        return m_id == other.m_id && m_version == other.m_version;
    }

    explicit constexpr operator bool() const noexcept
    {
        return m_id != invalid_id;
    }
};
} // namespace matter

#endif
