#ifndef MATTER_COMPONENT_METADATA_HPP
#define MATTER_COMPONENT_METADATA_HPP

#pragma once

#include <optional>
#include <string_view>

#include "matter/component/traits.hpp"

namespace matter
{
struct component_metadata
{
    std::optional<const std::string_view> name;

    std::size_t size;
    std::size_t align;

    // serialize, deserialize functions

    template<typename Component>
    constexpr component_metadata(std::in_place_type_t<Component>) noexcept
        : name{[]() -> std::optional<std::string_view> {
              if constexpr (matter::is_component_named_v<Component>)
              {
                  return matter::component_name_v<Component>;
              }
              else
              {
                  return std::nullopt;
              }
          }()},
          size{sizeof(Component)}, align{alignof(Component)}
    {}
};
} // namespace matter

#endif
