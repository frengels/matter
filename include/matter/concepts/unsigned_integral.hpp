#pragma once

#include "matter/concepts/signed_integral.hpp"

namespace matter
{
template<typename T>
concept unsigned_integral = integral<T> && !signed_integral<T>;
}