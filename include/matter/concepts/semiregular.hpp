#pragma once

#include "matter/concepts/copyable.hpp"
#include "matter/concepts/default_constructible.hpp"

namespace matter
{
template<typename T>
concept semiregular = copyable<T>&& default_constructible<T>;
}