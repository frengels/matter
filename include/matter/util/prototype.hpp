#ifndef MATTER_UTIL_PROTOTYPE_HPP
#define MATTER_UTIL_PROTOTYPE_HPP

#pragma once

#include "matter/util/concepts.hpp"

namespace matter
{
namespace prototype
{
struct optional
{
    explicit operator bool() const;
    int      operator*() const;
};

static_assert(matter::is_optional_v<matter::prototype::optional>);
} // namespace prototype
} // namespace matter

#endif
