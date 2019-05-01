#ifndef MATTER_ACCESS_REGISTRY_ACCESS_VIEW_HPP
#define MATTER_ACCESS_REGISTRY_ACCESS_VIEW_HPP

#pragma once

#include "matter/access/component_filter.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/component/group_vector.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
// same underlying id_type is guaranteed by the meta_access concept
template<typename Registry, typename... Access>
class registry_access_view {
public:
    using registry_type = Registry;
    using id_type       = typename registry_type::id_type;

    using group_vector_container_iterator_type =
        matter::iterator_t<std::vector<matter::group_vector<id_type>>>;
    using group_vector_container_sentinel_type =
        matter::sentinel_t<std::vector<matter::group_vector<id_type>>>;

    using required_types =
        matter::meta::merge_tuple_types_t<matter::required_types_t<
            matter::meta_type_t<Access, registry_type>>...>;

    template<typename... ReqTypes>
    using component_filter_type =
        matter::component_filter<registry_type, ReqTypes...>;

private:
    group_vector_container_iterator_type begin_;
    group_vector_container_sentinel_type end_;

    std::tuple<matter::meta_type_t<Access, Registry>...> meta_access_;

    // expands to matter::component_filter<Ts...>
    matter::meta::expand_tuple_type_t<component_filter_type, required_types>
        filter_;

public:
    constexpr registry_access_view(group_vector_container_iterator_type beg,
                                   group_vector_container_sentinel_type end,
                                   registry_type& reg) noexcept
        : begin_{std::move(beg)}, end_{std::move(end)},
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

        auto it = begin_ + min_group_size;
        if (it >= end_)
        {
            return f;
        }

        matter::for_each(it, end_, [&](matter::group_vector<id_type>& grp_vec) {
            std::apply(
                [&](auto&... meta) {
                    // not sure how to store the results of process_group_vector
                    // to pass on further yet
                    // just make the void ones a bool
                    auto process_group_vector = [](auto&& meta_access,
                                                   matter::group_vector<
                                                       id_type>& grp_vec) {
                        using type = std::remove_const_t<
                            std::remove_reference_t<decltype(meta_access)>>;

                        if constexpr (std::is_same_v<
                                          void,
                                          matter::process_group_vector_result_t<
                                              type>>)
                        {
                            return true;
                        }
                        else
                        {
                            return meta_access.process_group_vector(grp_vec);
                        }
                    };

                    auto process_grp_vec_res =
                        std::tuple{process_group_vector(meta, grp_vec)...};

                    auto proceed = std::apply(
                        [](const auto&... res) { return (bool(res) && ...); },
                        process_grp_vec_res);

                    // one of our filters didn't like the group_vector
                    if (!proceed)
                    {
                        return;
                    }

                    matter::for_each(
                        grp_vec.begin(),
                        grp_vec.end(),
                        [&](matter::any_group<id_type> grp) {
                            if (!filter_.filter(grp))
                            {
                                // if the filter wasn't successful return
                                // early
                                return;
                            }

                            auto process_group = [&](auto&& meta_access,
                                                     auto&& proc_grp_vec_res,
                                                     [[maybe_unused]] matter::
                                                         any_group<id_type>
                                                             grp) {
                                using type =
                                    std::remove_const_t<std::remove_reference_t<
                                        decltype(meta_access)>>;

                                // there is no return type so just return true
                                if constexpr (
                                    std::is_same_v<
                                        void,
                                        matter::process_group_result_t<type>>)
                                {
                                    return true;
                                }
                                // process_group_vector returned void or bool
                                // which are both already processed and don't
                                // need to be passed further
                                else if constexpr (
                                    std::is_same_v<
                                        bool,
                                        matter::process_group_vector_result_t<
                                            type>> ||
                                    std::is_same_v<
                                        void,
                                        matter::process_group_vector_result_t<
                                            type>>)
                                {
                                    return meta_access.process_group(grp);
                                }
                                // dereference the optional from
                                // process_group_vector and pass it on to
                                // process_group
                                else
                                {
                                    return meta_access.process_group(
                                        grp, *proc_grp_vec_res);
                                }
                            };

                            auto process_grp_res = std::apply(
                                [&](auto&... res) {
                                    return std::tuple{
                                        process_group(meta, res, grp)...};
                                },
                                process_grp_vec_res);

                            auto proceed = std::apply(
                                [](const auto&... res) {
                                    return (bool(res) && ...);
                                },
                                process_grp_res);

                            if (!proceed)
                            {
                                return;
                            }

                            auto size = grp.size();

#pragma omp simd
                            for (std::size_t i = 0; i < size; ++i)
                            {
                                auto handle = matter::entity_handle(grp, i);

                                auto access_tup = create_access_impl(
                                    handle,
                                    process_grp_res,
                                    std::index_sequence_for<Access...>{});

                                call_fn(f, access_tup);
                            }
                        });
                },
                meta_access_);
        });

        return f;
    }
};

template<typename... MetaAccess>
registry_access_view(
    matter::iterator_t<std::vector<matter::group_vector<
        typename matter::detail::first_t<MetaAccess...>::id_type>>> begin,
    matter::sentinel_t<std::vector<matter::group_vector<
        typename matter::detail::first_t<MetaAccess...>::id_type>>> end,
    MetaAccess&&... acc) noexcept
    ->registry_access_view<
        typename detail::first_t<MetaAccess...>::registry_type,
        typename matter::make_access_result<MetaAccess>::type...>;
} // namespace matter

#endif
