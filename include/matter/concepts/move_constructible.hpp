#pragma once

#include "matter/concepts/constructible_from.hpp"
#include "matter/concepts/convertible_to.hpp"

namespace matter
{
template<typename T>
concept move_constructible = constructible_from<T, T>&& convertible_to<T, T>;
}