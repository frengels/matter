#pragma once

#include <cassert>
#include <iterator>

#include <hera/algorithm/for_each.hpp>
#include <hera/algorithm/make_from_range.hpp>
#include <hera/algorithm/unpack.hpp>
#include <hera/container/pair.hpp>
#include <hera/container/tuple.hpp>
#include <hera/view/filter.hpp>
#include <hera/view/transform.hpp>
#include <hera/view/zip.hpp>

#include "matter/concepts/default_constructible.hpp"
#include "matter/concepts/same_as.hpp"
#include "matter/container/concepts.hpp"
#include "matter/container/erase_at.hpp"
#include "matter/container/soa_iterator.hpp"
#include "matter/container/soa_proxy.hpp"
#include "matter/container/soa_sentinel.hpp"
#include "matter/iterator/concepts.hpp"
#include "matter/ranges/concepts.hpp"
#include "matter/utility/decay_copy.hpp"

namespace matter
{
namespace detail
{
namespace impl
{
template<typename... Ts>
struct all_different_iter;

template<typename T, typename U, typename... Ts>
struct all_different_iter<T, U, Ts...>
    : std::bool_constant<!matter::same_as<T, U> &&
                         all_different_iter<T, Ts...>::value>
{};

template<typename T, typename U>
struct all_different_iter<T, U> : std::bool_constant<!matter::same_as<T, U>>
{};

template<typename... Ts>
struct all_different_start;

template<typename T, typename U, typename... Ts>
struct all_different_start<T, U, Ts...>
    : std::conjunction<all_different_iter<T, U, Ts...>,
                       all_different_start<U, Ts...>>
{};

template<typename T, typename U>
struct all_different_start<T, U> : all_different_iter<T, U>
{};

template<typename T>
struct all_different_start<T> : std::true_type
{};
} // namespace impl

// TODO: make this quicker using hashes
// this is very slow for now O(N^2) at compile time
template<typename T, typename... Ts>
concept all_different = impl::all_different_start<T, Ts...>::value;

template<typename... Conts>
constexpr bool all_containers_eraseable(hera::type_list<Conts...>) noexcept
{
    return (matter::eraseable<Conts> && ...);
}
} // namespace detail

template<matter::sized_range... Containers> // clang-format off
    requires(sizeof...(Containers) >= 1) && 
        (std::is_object_v<Containers> && ...) // clang-format on
    class soa {
    static_assert(
        (matter::same_as<Containers, std::remove_cvref_t<Containers>> && ...),
        "All containers must be their base type");
    static_assert(detail::all_different<range_value_t<Containers>...>,
                  "All containers must hold a unique value_type");

private:
    using this_type                       = matter::soa<Containers...>;
    static constexpr bool is_common_range = (common_range<Containers> && ...);

public:
    using size_type = matter::common_type_t<range_size_t<Containers>...>;

private:
    [[no_unique_address]] hera::tuple<Containers...> containers_;

public:
    soa() = default;

    template<typename... UContainers> // clang-format off
        requires sizeof...(UContainers) == sizeof...(Containers) &&
            (matter::sized_range<UContainers> && ...) // gcc can't handle this in the template param
            // https://godbolt.org/z/3MFk-z
    constexpr soa(UContainers&&... containers) // clang-format on
        noexcept(std::is_nothrow_constructible_v<hera::tuple<Containers...>,
                                                 UContainers...>)
        : containers_{std::forward<UContainers>(containers)...}
    {
        [[maybe_unused]] auto containers_same_size = [&] {
            auto expected_size = matter::size(containers_.front());
            bool sizes_match   = true;

            // auto conts = only_containers(containers_);
            hera::for_each(containers_, [&](auto& cont) {
                if (expected_size != matter::size(cont))
                {
                    sizes_match = false;
                }
            });

            return sizes_match;
        };

        assert(containers_same_size());
    }

    // check whether this soa contains the wanted value_type
    template<typename T>
    constexpr auto has_value_type() const noexcept
    {
        auto it = hera::find_if(containers_, [](const auto& container) {
            return std::is_same<
                T,
                range_value_t<std::remove_cvref_t<decltype(container)>>>{};
        });

        return it != hera::end(containers_);
    }

private:
    template<typename T> // clang-format off
        //requires decltype(has_value_type<T>())::value
    constexpr auto& container_for() noexcept // clang-format on
    {
        return *hera::find_if(containers_, [](auto& container) {
            return std::is_same<
                T,
                range_value_t<std::remove_reference_t<decltype(container)>>>{};
        });
    }

    template<typename T> // clang-format off
        //requires decltype(has_value_type<T>())::value
    constexpr const auto& container_for() const noexcept // clang-format on
    {
        return *hera::find_if(containers_, [](const auto& container) {
            return std::is_same<
                T,
                range_value_t<std::remove_cvref_t<decltype(container)>>>{};
        });
    }

public:
    template<typename... Ts> // clang-format off
        //requires (decltype(has_value_type<Ts>())::value && ...)
    constexpr decltype(auto) begin(hera::type_identity<Ts>...) // clang-format on
    {
        if constexpr (sizeof...(Ts) == 0)
        {
            // delegate to ourselves
            return begin(hera::type_identity<range_value_t<Containers>>{}...);
        }
        else
        {
            return soa_iterator{hera::type_identity<this_type>{},
                                matter::begin(container_for<Ts>())...};
        }
    }

    template<typename... Ts> // clang-format off
        //requires (decltype(has_value_type<Ts>())::value && ...)
    constexpr decltype(auto) end(hera::type_identity<Ts>...) const // clang-format on
    {
        if constexpr (sizeof...(Ts) == 0)
        {
            // delegate to ourselves
            return end(hera::type_identity<range_value_t<Containers>>{}...);
        }
        else
        {
            if constexpr (is_common_range)
            {
                // common_ranges return soa_iterator
                return soa_iterator{hera::type_identity<this_type>{},
                                    matter::end(container_for<Ts>())...};
            }
            else
            {
                // and not common_ranges return soa_iterator
                return soa_sentinel{hera::type_identity<this_type>{},
                                    matter::end(container_for<Ts>())...};
            }
        }
    }

    constexpr size_type size() const noexcept
    {
        // just use the first size function we find
        // possibly look for the first noexcept size function in the future.
        return static_cast<size_type>(matter::size(containers_.front()));
    }

    constexpr bool empty() const noexcept
    {
        // just reuse size()
        return size() == 0;
    }

    template<typename... Ts>
    constexpr void push_back(Ts&&... vals)
    {
        // TODO: make this a constraint of the function once gcc accepts it
        static_assert((matter::push_back_into<Ts, Containers> && ...));

        hera::unpack(containers_, [&](auto&... conts) {
            (conts.push_back(std::forward<Ts>(vals)), ...);
        });
    }

    template<typename... Ts>
    constexpr void push_back(const soa_proxy<Ts...>& val)
    {}

    template<typename... Ts>
    constexpr void push_back(soa_proxy<Ts...>&& val)
    {}

private:
    template<std::size_t I,
             std::size_t... Is,
             typename Tuple> // clang-format off
        requires
            requires(Tuple&& tup)
            {
                requires decltype(hera::size(tup))::value == sizeof...(Is); 
            }
    constexpr void emplace_back_one_impl(std::index_sequence<Is...>, // clang-format on
        Tuple&&
            tup) noexcept(noexcept(containers_
                                       [std::integral_constant<std::size_t,
                                                               I>{}]
                                           .emplace_back(std::forward<Tuple>(
                                               tup)[std::integral_constant<
                                               std::size_t,
                                               Is>{}]...)))
    {
        containers_[std::integral_constant<std::size_t, I>{}].emplace_back(
            std::forward<Tuple>(
                tup)[std::integral_constant<std::size_t, Is>{}]...);
    }

    template<std::size_t I, typename Tuple>
    constexpr void
    emplace_back_one(Tuple&& tup) noexcept(noexcept(emplace_back_one_impl<I>(
        std::make_index_sequence<decltype(hera::size(tup))::value>{},
        std::forward<Tuple>(tup))))
    {
        emplace_back_one_impl<I>(
            std::make_index_sequence<decltype(hera::size(tup))::value>{},
            std::forward<Tuple>(tup));
    }

    template<std::size_t... Is, typename... Tuples> // clang-format off
        requires sizeof...(Containers) == sizeof...(Tuples) &&
            sizeof...(Is) == sizeof...(Tuples)
    constexpr void emplace_back_impl(std::index_sequence<Is...>, Tuples&&... tups) // clang-format on
            noexcept(noexcept((emplace_back_one<Is>(std::forward<Tuples>(tups)),
                               ...)))
    {
        (emplace_back_one<Is>(std::forward<Tuples>(tups)), ...);
    }

public:
    template<typename... Tuples> // clang-format off
        requires sizeof...(Containers) == sizeof...(Tuples)
    constexpr void emplace_back(Tuples&&... tups) // clang-format on
        noexcept(noexcept(
            emplace_back_impl(std::make_index_sequence<sizeof...(Tuples)>{},
                              std::forward<Tuples>(tups)...)))
    {
        // we can't simply use make_from_tuple because this would invoke a move.
        // We want to forward all parameters from the tuple to the respective
        // container's emplace_back method.
        emplace_back_impl(std::make_index_sequence<sizeof...(Tuples)>{},
                          std::forward<Tuples>(tups)...);
    }

    template<
        typename ContList = hera::type_list<Containers...>> // clang-format off
        requires
            requires(ContList list)
            {
                detail::all_containers_eraseable(list);
            }
    constexpr soa_iterator<this_type, iterator_t<Containers>...>
    erase(const soa_iterator<this_type, iterator_t<Containers>...>& pos) // clang-format on
    {
        return hera::unpack(
            hera::zip_view{containers_, pos.iterators_},
            [](auto&&... cont_it_pair) {
                return soa_iterator<this_type,
                                    iterator_t<Containers>...>{matter::erase_at(
                    std::forward<decltype(cont_it_pair)>(cont_it_pair).front(),
                    std::forward<decltype(cont_it_pair)>(cont_it_pair)
                        .back())...};
            });
    }
};

template<typename... UContainers>
soa(UContainers&&...)->soa<std::decay_t<UContainers>...>;
} // namespace matter