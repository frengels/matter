#pragma once

#include "matter/concepts/convertible_to.hpp"
#include "matter/concepts/same_as.hpp"
#include "matter/utility/common_reference.hpp"

namespace matter
{
template<typename T, typename U>
concept common_reference_with =
    same_as<common_reference_t<T, U>, common_reference_t<U, T>>&&
            convertible_to<T, common_reference_t<T, U>>&&
            convertible_to<U, common_reference_t<T, U>>;
}