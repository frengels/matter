#ifndef MATTER_ACCESS_REGISTRY_ACCESS_VIEW_HPP
#define MATTER_ACCESS_REGISTRY_ACCESS_VIEW_HPP

#pragma once

#include "matter/access/component_filter.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
// same underlying id_type is guaranteed by the meta_access concept
template<typename Registry, typename... Access>
class registry_access_view {
public:
    using registry_type        = Registry;
    using id_type              = typename registry_type::id_type;
    using group_container_type = typename registry_type::group_container_type;

    using required_types =
        matter::meta::merge_tuple_types_t<matter::required_types_t<
            matter::meta_type_t<Access, registry_type>>...>;

    template<typename... ReqTypes>
    using component_filter_type =
        matter::component_filter<registry_type, ReqTypes...>;

private:
    std::reference_wrapper<group_container_type> container_;

    std::tuple<matter::meta_type_t<Access, Registry>...> meta_access_;

    // expands to matter::component_filter<Ts...>
    matter::meta::expand_tuple_type_t<component_filter_type, required_types>
        filter_;

public:
    constexpr registry_access_view(registry_type& reg) noexcept
        : container_{reg.group_container()},
          meta_access_{matter::meta_type_t<Access, registry_type>{reg}...},
          filter_{reg}
    {}

private:
    template<typename... ProcGroupRes, std::size_t... Is>
    constexpr auto create_access_impl(matter::entity_handle<id_type> ent,
                                      std::tuple<ProcGroupRes...>& proc_res_tup,
                                      std::index_sequence<Is...>) noexcept
    {
        return std::tuple{filter_.make_access(
            std::get<Is>(meta_access_), ent, std::get<Is>(proc_res_tup))...};
    }

    template<typename Function>
    constexpr void call_fn(Function&              fn,
                           std::tuple<Access...>& access_tuple) noexcept
    {
        fn(std::get<Access>(access_tuple)...);
    }

public:
    template<typename Function>
    Function for_each(Function f) noexcept
    {
        constexpr auto min_group_size = std::tuple_size_v<required_types>;

        auto& container        = container_.get();
        auto  max_present_size = container.groups_size();

        auto curr_grp_size = min_group_size;

        if (curr_grp_size > max_present_size)
        {
            return f;
        }

        for (; curr_grp_size <= max_present_size; ++curr_grp_size)
        {
            auto rng = container.range(curr_grp_size);

            matter::for_each(
                rng.begin(), rng.end(), [&](matter::any_group<id_type> grp) {
                    if (!filter_.filter(grp))
                    {
                        // if the filter wasn't successful return
                        // early
                        return;
                    }

                    std::apply(
                        [&](auto&... meta) {
                            auto process_group =
                                [](auto& meta,
                                   [[maybe_unused]] matter::any_group<id_type>
                                       grp) {
                                    using current_meta_type =
                                        std::decay_t<decltype(meta)>;
                                    if constexpr (!matter::processes_group_v<
                                                      current_meta_type>)
                                    {
                                        return true;
                                    }
                                    else if constexpr (
                                        std::is_same_v<
                                            void,
                                            matter::process_group_result_t<
                                                current_meta_type>>)
                                    {
                                        return true;
                                    }
                                    else
                                    {
                                        return meta.process_group(grp);
                                    }
                                };
                            auto process_grp_res =
                                std::tuple{process_group(meta, grp)...};

                            bool proceed = std::apply(
                                [](auto&&... res) {
                                    return (bool(res) && ...);
                                },
                                process_grp_res);

                            // a processing condition failed
                            if (!proceed)
                            {
                                return;
                            }

                            auto size = grp.size();

#pragma omp simd
                            for (decltype(size) it = 0; it < size; ++it)
                            {
                                auto ent_handle =
                                    matter::entity_handle(grp, it);

                                auto access_tup = create_access_impl(
                                    ent_handle,
                                    process_grp_res,
                                    std::index_sequence_for<Access...>{});

                                call_fn(f, access_tup);
                            }
                        },
                        meta_access_);
                });
        }

        return f;
    }
}; // namespace matter
} // namespace matter

#endif
