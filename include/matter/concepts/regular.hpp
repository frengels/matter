#pragma once

#include "matter/concepts/equality_comparable.hpp"
#include "matter/concepts/semiregular.hpp"

namespace matter
{
template<typename T>
concept regular = semiregular<T>&& equality_comparable<T>;
}