#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "component_identifier.hpp"

#include "matter/util/erased.hpp"

namespace matter
{
template<typename... Components>
class registry {
public:
    template<std::size_t I>
    class group {
    public:
        static constexpr auto num_components = I;

        using id_type = typename component_identifier<Components...>::id_type;

    private:
        std::array<std::pair<id_type, erased>, num_components> storage_;

    public:
        template<typename... Pairs>
        constexpr group(Pairs&&... id_storage_pairs) noexcept
            : storage_{std::forward<Pairs>(id_storage_pairs)...}
        {
            // passed stores must be sorted by id
            assert(is_sorted());

            // both of these together make ==, this just gives nicer error
            // messages.
            static_assert(sizeof...(Pairs) <= num_components,
                          "Cannot construct with this many storages.");
            static_assert(sizeof...(Pairs) >= num_components,
                          "Cannot construct with this few storages.");
        }

        constexpr bool contains(id_type id) const noexcept
        {
            auto index_it = find_id(id);
            return index_it != storage_.end();
        }

        template<typename C>
        matter::component_storage_t<C>& get(id_type id) noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->second
                .template get<matter::component_storage_t<C>>();
        }

        template<typename C>
        const matter::component_storage_t<C>& get(id_type id) const noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->second
                .template get<matter::component_storage_t<C>>();
        }

    private:
        /// \brief checks whether the stores are sorted
        constexpr auto is_sorted() const noexcept
        {
            return std::is_sorted(storage_.begin(),
                                  storage_.end(),
                                  [](const auto& lhs, const auto& rhs) {
                                      return lhs.first < rhs.first;
                                  });
        }

        constexpr auto find_id(id_type id) noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(),
                                 storage_.end(),
                                 id,
                                 [](const auto& lhs, const auto& rhs) {
                                     return lhs.first < rhs;
                                 });

            if (index_it->first != id)
            {
                return storage_.end();
            }

            return index_it;
        }

        constexpr auto find_id(id_type id) const noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(),
                                 storage_.end(),
                                 id,
                                 [](const auto& lhs, const auto& rhs) {
                                     return lhs.first < rhs;
                                 });

            if (index_it->first != id)
            {
                return storage_.end();
            }

            return index_it;
        }
    };

public:
    using identifier_type = component_identifier<Components...>;
    using id_type         = typename identifier_type::id_type;

private:
    identifier_type identifier_;

public:
    constexpr registry() noexcept = default;

    template<typename C>
    constexpr id_type component_id() const noexcept
    {
        return identifier_.template id<C>();
    }
};
} // namespace matter

#endif
