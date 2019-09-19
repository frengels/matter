#pragma once

#include "matter/concepts/assignable_from.hpp"
#include "matter/concepts/move_constructible.hpp"

namespace matter
{
template<typename T>
concept movable =
    std::is_object_v<T>&& move_constructible<T>&& assignable_from<T&, T>;
// swappable<T>
} // namespace matter