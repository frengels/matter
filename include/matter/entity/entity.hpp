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

private:
};
} // namespace matter

#endif
