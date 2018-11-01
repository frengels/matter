#ifndef MATTER_COMPONENT_COMPONENT_TRAITS_HPP
#define MATTER_COMPONENT_COMPONENT_TRAITS_HPP

#pragma once

#include <type_traits>

namespace matter
{
template<typename T, typename = void>
struct is_component_traits : public std::false_type
{};

  //template<typename T, typename = std::void_t<
} // namespace matter

#endif
