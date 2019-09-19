#pragma once

#include "matter/concepts/constructible_from.hpp"
#include "matter/concepts/convertible_to.hpp"
#include "matter/concepts/move_constructible.hpp"

namespace matter
{
template<typename T>
concept copy_constructible =
    move_constructible<T>&& constructible_from<T, T&>&& convertible_to<T&, T>&&
        constructible_from<T, const T&>&& convertible_to<const T&, T>&&
            constructible_from<T, const T>&& convertible_to<const T, T>;
}