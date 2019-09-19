#pragma once

#include <type_traits>

namespace matter
{
template<typename T>
concept destructible = std::is_nothrow_destructible_v<T>;
}