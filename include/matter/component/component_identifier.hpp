#ifndef MATTER_COMPONENT_COMPONENT_IDENTIFIER_HPP
#define MATTER_COMPONENT_COMPONENT_IDENTIFIER_HPP

#pragma once

#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "matter/component/identifier.hpp"
#include "matter/component/metadata.hpp"
#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"

namespace matter
{

struct unregistered_component : std::logic_error
{
    template<typename Component>
    unregistered_component(std::in_place_type_t<Component>)
        : std::logic_error{[]() {
              std::stringstream ss;
              ss << "Component \"";
              if constexpr (matter::is_component_named_v<Component>)
              {
                  ss << matter::component_name_v<Component>;
              }
              else
              {
                  ss << "unknown";
              }
              ss << "\" was not registered";
              return std::move(ss).str();
          }()}
    {}
};

/// \brief identifies components by an id
/// Each component must be assigned an id, there is the struct identifier which
/// assists in generating a `size_t` id for the component. We cannot use this
/// generated id directly because these generated ids are global, with them
/// being global if we use them directly there might be big gaps in ids in case
/// you use multiple instances of `component_identifier`. To avoid this we map
/// the global ids to a local id. This way we get a clean array of ids.
template<typename... Components>
class component_identifier {
    static_assert((matter::is_component_v<Components> && ...),
                  "All types must be valid components");

    struct component_identifier_tag
    {};

public:
    using id_type = std::size_t;

    static constexpr auto constexpr_components_size = sizeof...(Components);

    using identifier_type = identifier<component_identifier_tag>;

private:
    /// holds all the runtime ids, the key is the id generated by the
    /// identifier and the value is the id used by the local
    /// component_identifier
    std::unordered_map<std::size_t, std::size_t> runtime_ids_;
    std::size_t next_local_id_{constexpr_components_size};

    std::array<matter::component_metadata, constexpr_components_size>
                                            static_metadata_;
    std::vector<matter::component_metadata> runtime_metadata_;

public:
    constexpr component_identifier()
        : static_metadata_{
              matter::component_metadata{std::in_place_type_t<Components>{}}...}
    {}

    template<typename Component>
    static constexpr bool is_static() noexcept
    {
        return detail::type_in_list_v<Component, Components...>;
    }

    /// \brief instructs the identifier to now identify this component
    template<typename Component>
    std::size_t register_type() noexcept
    {
        static_assert(matter::is_component_v<Component>,
                      "You may only register valid components");
        auto id = identifier_type::template get<Component>();
        assert(runtime_ids_.count(id) ==
               0); // if not 0 then we already identify this type
        auto local_id = next_local_id_++;
        runtime_ids_.emplace(id, local_id);
        // store all available metadata for id -> data relation
        runtime_metadata_.emplace_back(std::in_place_type_t<Component>{});
        return local_id;
    }

    const matter::component_metadata& metadata(id_type id) noexcept
    {
        assert((constexpr_components_size + runtime_metadata_.size()) > id);

        if (id >= constexpr_components_size)
        {
            auto runtime_id = id - constexpr_components_size;
            return runtime_metadata_[runtime_id];
        }
        else
        {
            return static_metadata_[id];
        }
    }

    template<typename Component>
    constexpr bool is_registered() const noexcept
    {
        if constexpr (is_static<Component>())
        {
            return true;
        }

        auto id = identifier_type::template get<Component>();
        return runtime_ids_.count(id) == 1;
    }

    /// \brief retrieve the local id for a component
    template<typename Component>
    constexpr auto id() const
    {
        if constexpr (is_static<Component>())
        {
            return static_id<Component>();
        }
        else
        {
            return runtime_id<Component>();
        }
    }

    template<typename... Cs>
    constexpr auto ids() const
        -> unordered_typed_ids<id_type, decltype(id<Cs>())...>
    {
        return {id<Cs>()...};
    }

    template<typename... Cs>
    constexpr auto ordered_ids() const
        -> ordered_typed_ids<id_type, decltype(id<Cs>())...>
    {
        return {ids<Cs...>()};
    }

private:
    template<typename Component>
    constexpr auto static_id() const noexcept
    {
        static_assert(
            is_static<Component>(),
            "This component id should be retrieved using runtime_id() instead");
        constexpr auto res =
            detail::type_index<Component, Components...>().value();
        return matter::static_id<id_type, Component, res>{};
    }

    template<typename Component>
    constexpr auto runtime_id() const
    {
        static_assert(
            !is_static<Component>(),
            "This component id should be retrieve using static_id() instead");
        auto id = identifier_type::template get<Component>();

        auto it = runtime_ids_.find(id);

        if (it == runtime_ids_.end())
        {
            // throw in case of non registered component
            throw matter::unregistered_component{
                std::in_place_type_t<Component>{}};
        }

        return matter::runtime_id<id_type, Component>{it->second};
    }
};
} // namespace matter

#endif
