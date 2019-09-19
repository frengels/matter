#pragma once

#include "matter/concepts/assignable_from.hpp"
#include "matter/concepts/copy_constructible.hpp"
#include "matter/concepts/movable.hpp"

namespace matter
{
template<typename T>
concept copyable =
    copy_constructible<T>&& movable<T>&& assignable_from<T&, const T&>;
}