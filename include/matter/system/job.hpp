#ifndef MATTER_SYSTEM_JOB_HPP
#define MATTER_SYSTEM_JOB_HPP

#pragma once

#include <type_traits>

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/type.hpp>

#include "matter/query/processor.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Job, typename World>
constexpr void invoke_job(Job& job, World&& world) noexcept
{
    using boost::hana::type_c;

    // retrieve the instantiated queries
    auto& queries = job.queries();

    boost::hana::unpack(
        queries, [&](auto&&... query) { job(process_query(query, world)...); });
}

template<typename T, typename World, typename = void>
struct is_job_for_world : std::false_type
{};

template<typename T, typename World>
struct is_job_for_world<
    T,
    World,
    std::void_t<decltype(std::declval<T&>().queries()),
                decltype(matter::invoke_job(std::declval<T&>(),
                                            std::declval<World&>()))>>
    : std::true_type
{};

template<typename T, typename World>
constexpr bool is_job_for_world_v = is_job_for_world<T, World>::value;

namespace detail
{
template<typename UpdateFn, typename... Query>
class simple_job {
private:
    [[no_unique_address]] UpdateFn             update_fn_;
    [[no_unique_address]] std::tuple<Query...> queries_;

public:
    constexpr simple_job(UpdateFn fn,
                         boost::hana::basic_type<Query>...) noexcept
        : update_fn_{std::move(fn)}, queries_{Query{}...}
    {}

    constexpr std::tuple<Query...>& queries() noexcept
    {
        return queries_;
    }

    constexpr const std::tuple<Query...>& queries() const noexcept
    {
        return queries_;
    }

    template<typename... QueryResults>
    constexpr void operator()(QueryResults&&... res)
    {
        update_fn_(std::forward<QueryResults>(res)...);
    }
};
} // namespace detail

template<typename... Queries, typename UpdateFn>
constexpr decltype(auto) make_job(UpdateFn fn) noexcept
{
    return detail::simple_job{std::move(fn), boost::hana::type_c<Queries>...};
}
} // namespace matter

#endif
