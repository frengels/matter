#ifndef MATTER_UTIL_META_HPP
#define MATTER_UTIL_META_HPP

#pragma once

namespace matter
{
namespace detail
{
template<typename T1, typename T2, typename U = void>
using enable_if_same_t = std::enable_if_t<std::is_same_v<T1, T2>, U>;
}
} // namespace matter

#endif
