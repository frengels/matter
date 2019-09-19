#pragma once

#include "matter/concepts/constructible_from.hpp"

namespace matter
{
template<typename T>
concept default_constructible = constructible_from<T>;
}