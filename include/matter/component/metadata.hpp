#ifndef MATTER_COMPONENT_METADATA_HPP
#define MATTER_COMPONENT_METADATA_HPP

#pragma once

#include <optional>
#include <string_view>

namespace matter
{
struct component_metadata
{
    std::optional<const std::string_view> name;

    constexpr component_metadata(std::optional<const std::string_view> name)
        : name{name}
    {}
};
} // namespace matter

#endif
