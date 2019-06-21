#ifndef MATTER_ID_COMPONENT_IDENTIFIER_HPP
#define MATTER_ID_COMPONENT_IDENTIFIER_HPP

#pragma once

#include <type_traits>

#include "matter/component/prototype.hpp"
#include "matter/id/typed_id.hpp"

namespace matter
{
template<typename T, typename = void>
struct is_component_identifier : std::false_type
{};

// a component identifier generates a unique id for each component registered
template<typename T>
struct is_component_identifier<
    T,
    std::enable_if_t<
        std::is_same_v<
            matter::typed_id<typename T::id_type, matter::prototype::component>,
            decltype(
                std::declval<T>()
                    .template register_type<matter::prototype::component>())> &&
            std::is_same_v<bool,
                           decltype(std::declval<const T>()
                                        .template is_registered<
                                            matter::prototype::component>())> &&
            std::is_same_v<
                matter::typed_id<typename T::id_type,
                                 matter::prototype::component>,
                decltype(std::declval<const T>()
                             .template id<matter::prototype::component>())>,
        std::void_t<typename T::id_type>>> : std::true_type
{};

template<typename T>
constexpr bool is_component_identifier_v = is_component_identifier<T>::value;
} // namespace matter

#endif
