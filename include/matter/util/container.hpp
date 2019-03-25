#ifndef MATTER_UTIL_CONTAINER_HPP
#define MATTER_UTIL_CONTAINER_HPP

#pragma once

#include <utility>

namespace matter
{
namespace impl
{
template<std::size_t... Is, typename Container, typename TArgs>
decltype(auto)
emplace_back_tuple_impl(std::index_sequence<Is...>, Container& c, TArgs&& args)
{
    // they're already forwarded in the tuple, only forward the tuple
    return c.emplace_back(std::get<Is>(std::forward<TArgs>(args))...);
}
} // namespace impl

template<typename Container, typename TArgs>
decltype(auto) emplace_back_tuple(Container& c, TArgs&& args)
{
    impl::emplace_back_tuple_impl(
        std::make_index_sequence<
            std::tuple_size<std::remove_reference_t<TArgs>>::value>{},
        c,
        std::forward<TArgs>(args));
}
} // namespace matter

#endif
