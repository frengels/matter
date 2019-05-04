#ifndef MATTER_ACCESS_ENTITY_HANDLE_HPP
#define MATTER_ACCESS_ENTITY_HANDLE_HPP

#pragma once

#include "matter/component/any_group.hpp"

namespace matter
{
/// \brief refers to an entity within the registry
/// This handle is only valid until the next mutation has happened on the
/// registry. Any usage after this point is undefined behavior.
template<typename Id>
class entity_handle {
public:
    using id_type = Id;

private:
    any_group<id_type> grp_;
    std::size_t        index_;

public:
    constexpr entity_handle(any_group<id_type> grp, std::size_t index) noexcept
        : grp_{std::move(grp)}, index_{index}
    {}

    any_group<id_type> group() noexcept
    {
        return grp_;
    }

    std::size_t index() const noexcept
    {
        return index_;
    }

    void erase() noexcept
    {
        grp_.erase(index_);
    }
};
} // namespace matter

#endif
