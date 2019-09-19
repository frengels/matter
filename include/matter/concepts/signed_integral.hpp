#pragma once

#include "matter/concepts/integral.hpp"

namespace matter
{
template<typename T>
concept signed_integral = matter::integral<T>&& std::is_signed_v<T>;
}