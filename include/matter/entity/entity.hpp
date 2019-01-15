#ifndef MATTER_ENTITY_HPP
#define MATTER_ENTITY_HPP

#pragma once

#include <cstdint>
#include <queue>
#include <type_traits>
#include <vector>

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
    using id_type         = Tid;
    using generation_type = Tversion;

    static constexpr id_type invalid_id = -1;

private:
    id_type         m_id;
    generation_type m_generation;

public:
    constexpr entity(const id_type& id, const generation_type& ver) noexcept
        : m_id{id}, m_generation{ver}
    {}

    constexpr entity(const entity&) noexcept = default;
    constexpr entity& operator=(const entity&) noexcept = default;
    constexpr entity(entity&&) noexcept                 = default;
    constexpr entity& operator=(entity&&) noexcept = default;

    constexpr id_type id() const noexcept
    {
        return m_id;
    }

    constexpr generation_type generation() const noexcept
    {
        return m_generation;
    }

    constexpr bool operator==(const entity& other) const noexcept
    {
        return m_id == other.m_id && m_generation == other.m_generation;
    }

    constexpr bool operator!=(const entity& other) const noexcept
    {
        return !(*this == other);
    }

    explicit constexpr operator bool() const noexcept
    {
        return m_id != invalid_id;
    }
};

using default_entity = entity<uint32_t, uint32_t>;
} // namespace matter

#endif
