#ifndef MATTER_ACCESS_COMPONENT_FILTER_HPP
#define MATTER_ACCESS_COMPONENT_FILTER_HPP

#pragma once

#include <utility>

#include "matter/access/entity_handle.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/component/any_group.hpp"

namespace matter
{
template<typename Registry, typename... Cs>
class component_filter {
public:
    using registry_type = Registry;

    using unordered_ids_type =
        decltype(std::declval<registry_type>().template component_ids<Cs...>());

private:
    unordered_ids_type                              tids_;
    matter::any_group                               filtered_grp_;
    std::tuple<matter::component_storage_t<Cs>*...> stores_;

public:
    constexpr component_filter(registry_type& reg)
        : tids_{reg.template component_ids<Cs...>()}
    {}

    constexpr bool filter(matter::any_group grp) noexcept
    {
        filtered_grp_ = std::move(grp);

        // retrieve
        auto er_stores =
            std::tuple{filtered_grp_.find_id(tids_.template get<Cs>())...};

        // if any are null then we can safely abort
        if (std::apply(
                [](auto*... er_stores) {
                    return ((er_stores == nullptr) || ...);
                },
                er_stores))
        {
            filtered_grp_ = matter::any_group{};
            return false;
        }

        // in case all erased_stores are found then populate the tuple
        stores_ = std::apply(
            [](auto*... er_stores) {
                return std::tuple{
                    std::addressof(er_stores->template get<Cs>())...};
            },
            er_stores);
        return true;
    }

    template<typename T>
    constexpr matter::component_storage_t<T>& get() noexcept
    {
        assert(bool(filtered_grp_));
        return *std::get<matter::component_storage_t<T>*>(stores_);
    }

    template<typename T>
    constexpr const matter::component_storage_t<T>& get() const noexcept
    {
        assert(bool(filtered_grp_));
        return *std::get<matter::component_storage_t<T>*>(stores_);
    }

private:
    template<typename MetaAccess, typename ProcGroupRes, std::size_t... Is>
    auto make_access_impl(MetaAccess&                    meta,
                          matter::entity_handle          ent,
                          [[maybe_unused]] ProcGroupRes& proc_res,
                          std::index_sequence<Is...>) noexcept
    {
        using req_types = matter::required_types_t<MetaAccess>;

        static_assert(std::tuple_size_v<req_types> == sizeof...(Is),
                      "Must be same length");

        // should always be true when we get to this point
        assert(bool(proc_res));

        if constexpr (matter::is_optional_v<
                          matter::process_group_result_t<MetaAccess>>)
        {
            return meta.make_access(
                ent,
                *proc_res,
                matter::storage_handle{
                    get<matter::meta::nth_tuple_type_t<Is, req_types>>()}...);
        }
        else
        {
            return meta.make_access(
                ent,
                matter::storage_handle<
                    matter::meta::nth_tuple_type_t<Is, req_types>>{
                    get<matter::meta::nth_tuple_type_t<Is, req_types>>()}...);
        }
    }

public:
    template<typename MetaAccess, typename ProcGroupRes>
    auto make_access(MetaAccess&           meta,
                     matter::entity_handle ent,
                     ProcGroupRes&         proc_res) noexcept
    {
        return make_access_impl(
            meta,
            std::move(ent),
            proc_res,
            std::make_index_sequence<
                std::tuple_size_v<matter::required_types_t<MetaAccess>>>{});
    }
};
} // namespace matter

#endif
