#pragma once

#include <type_traits>

namespace matter
{
template<typename T>
concept integral = std::is_integral_v<T>;
}