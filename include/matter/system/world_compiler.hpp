#ifndef MATTER_SYSTEM_WORLD_COMPILER_HPP
#define MATTER_SYSTEM_WORLD_COMPILER_HPP

#pragma once

#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/type.hpp>

#include "matter/id/id_cache.hpp"
#include "matter/query/category.hpp"
#include "matter/query/type_traits.hpp"

namespace matter
{
// collects information to speed up statically known information for queries and
// handles all the registration of required components
template<typename World, typename... Queries>
class world_compiler {
public:
    using id_type = typename World::id_type;

private:
    // this function extracts all required types from the queries to generate an
    // ideal id_cache for all entity queries.
    static constexpr auto extract_component_types = []() {
        using boost::hana::type_c;

        auto types              = boost::hana::tuple_t<Queries...>;
        auto entity_query_types = boost::hana::filter(types, [](auto t) {
            return boost::hana::integral_c<
                bool,
                matter::traits::is_entity_query(
                    type_c<typename decltype(t)::type>)>;
        });

        auto type_queries =
            boost::hana::transform(entity_query_types, [](auto eq_type) {
                return matter::traits::entity_query_type_queries(eq_type);
            });

        // remove the surrounding tuples
        auto flattened_type_queries = boost::hana::flatten(type_queries);

        auto component_types =
            boost::hana::transform(flattened_type_queries, [](auto type_query) {
                return type_c<typename decltype(
                    type_query)::type::element_type>;
            });

        auto unique_type_queries = boost::hana::to_set(component_types);

        return unique_type_queries;
    };

    static constexpr auto create_id_cache = [](World& w,
                                               auto&& component_types) {
        return boost::hana::unpack(
            std::forward<decltype(component_types)>(component_types),
            [&](auto&&... comp_types) {
                return matter::id_cache(w, comp_types...);
            });
    };

private:
    using component_type_list_type = decltype(extract_component_types());
    using id_cache_type =
        decltype(create_id_cache(std::declval<World&>(),
                                 std::declval<component_type_list_type>()));

private:
    World* world_;

    [[no_unique_address]] component_type_list_type component_types_{};

    id_cache_type comp_id_cache_;

public:
    constexpr world_compiler(World& w,
                             boost::hana::basic_type<Queries>...) noexcept
        : world_{std::addressof(w)},
          component_types_{extract_component_types()}, comp_id_cache_{[&]() {
              auto try_register = [&](auto comp_type) {
                  using component_type = typename decltype(comp_type)::type;

                  // register all component types to the identifier
                  if (!w.template contains_component<component_type>())
                  {
                      w.template register_component<component_type>();
                  }
              };

              // construct the id_cache, to get O(1) component id access
              return boost::hana::unpack(
                  component_types_, [&](auto&&... comp_types) {
                      (try_register(comp_types), ...);

                      return matter::id_cache{w, comp_types...};
                  });
          }()}
    {}

    template<typename T>
    constexpr auto component_id() const
        noexcept(noexcept(comp_id_cache_.template component_id<T>()))
            -> std::enable_if_t<
                matter::is_component_identifier_for_v<id_cache_type, T>,
                decltype(comp_id_cache_.template component_id<T>())>
    {
        return comp_id_cache_.template component_id<T>();
    }

    template<typename T>
    constexpr auto contains_component() const
        noexcept(noexcept(comp_id_cache_.template contains_component<T>()))
    {
        return comp_id_cache_.template contains_component<T>();
    }

    constexpr decltype(auto)
    group_range() noexcept(noexcept(world_->group_range()))
    {
        return world_->group_range();
    }
};
} // namespace matter

#endif
