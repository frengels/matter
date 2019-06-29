#ifndef MATTER_COMPONENT_PROTOTYPE_HPP
#define MATTER_COMPONENT_PROTOTYPE_HPP

#pragma once

#include "matter/component/traits.hpp"

namespace matter
{
namespace prototype
{
struct component
{
    component();
};

static_assert(matter::is_component_v<matter::prototype::component>);
} // namespace prototype
} // namespace matter

#endif
