#pragma once

#include <type_traits>

namespace matter
{
template<typename T>
concept floating_point = std::is_floating_point_v<T>;
}