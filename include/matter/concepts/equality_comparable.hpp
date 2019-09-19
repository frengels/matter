#pragma once

#include "matter/concepts/weakly_equality_comparable_with.hpp"

namespace matter
{
template<typename T>
concept equality_comparable = weakly_equality_comparable_with<T, T>;
}