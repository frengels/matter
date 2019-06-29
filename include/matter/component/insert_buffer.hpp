#ifndef MATTER_COMPONENT_INSERT_BUFFER_HPP
#define MATTER_COMPONENT_INSERT_BUFFER_HPP

#pragma once

#include <vector>

#include "matter/id/typed_id.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename Id, typename... Ts>
struct insert_buffer
{
    using id_type = Id;

private:
    template<typename _Id, typename... Us>
    friend struct insert_buffer;

private:
    matter::unordered_typed_ids<Id, Ts...> ids_;
    std::tuple<std::vector<Ts>...>         buffers_;

public:
    insert_buffer(const matter::unordered_typed_ids<Id, Ts...>& ids) noexcept
        : ids_{ids}, buffers_{}
    {}

    template<typename... Us>
    insert_buffer(const matter::unordered_typed_ids<Id, Ts...>& ids,
                  matter::insert_buffer<Id, Us...>&& move_from) noexcept
        : ids_{ids}, buffers_{[&]() {
              // this will take the vector from the passed buffer and use any
              // buffer that we also have in the current buffer
              if constexpr (detail::type_in_list_v<Ts, Us...>)
              {
                  auto vec =
                      std::vector<Ts>{std::move(move_from.template get<Ts>())};
                  vec.clear();
                  return vec;
              }
              else
              {
                  return std::vector<Ts>{};
              }
          }()...}
    {}

    template<typename T>
    constexpr auto begin() noexcept
    {
        return get<T>().begin();
    }

    template<typename T>
    constexpr auto end() noexcept
    {
        return get<T>().end();
    }

    template<typename T>
    constexpr auto begin() const noexcept
    {
        return get<T>().begin();
    }

    template<typename T>
    constexpr auto end() const noexcept
    {
        return get<T>().end();
    }

    const auto& ids() const noexcept
    {
        return ids_;
    }

    std::size_t size() const noexcept
    {
        // all buffers are guaranteed to be the same size
        return std::get<0>(buffers_).size();
    }

    void reserve(std::size_t new_capacity) noexcept
    {
        return std::apply(
            [&](auto&&... buffers) { (buffers.reserve(new_capacity), ...); },
            buffers_);
    }

    void resize(std::size_t new_size) noexcept(
        (std::is_nothrow_default_constructible_v<Ts> && ...))
    {
        return std::apply(
            [&](auto&&... buffers) { (buffers.resize(new_size), ...); },
            buffers_);
    }

    void resize(std::size_t new_size, const Ts&... ts) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> && ...))
    {
        (get<Ts>().resize(new_size, ts), ...);
    }

    template<typename... TupArgs>
    std::enable_if_t<(detail::is_constructible_expand_tuple_v<Ts, TupArgs> &&
                      ...)>
    emplace_back(TupArgs&&... tuple_args) noexcept(
        (detail::is_nothrow_constructible_expand_tuple_v<Ts, TupArgs> && ...))
    {
        (emplace_one_impl<Ts>(std::forward<TupArgs>(tuple_args)), ...);
    }

    template<typename... Us>
    std::enable_if_t<sizeof...(Us) == sizeof...(Ts) &&
                     (std::is_constructible_v<Ts, Us> && ...)>
    emplace_back(Us&&... us) noexcept(
        (std::is_nothrow_constructible_v<Ts, Us> && ...))
    {
        (get<Ts>().emplace_back(std::forward<Us>(us)), ...);
    }

private:
    template<typename T, typename... Args>
    void emplace_one_impl(std::tuple<Args...> tuple_args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        get<T>().emplace_back(std::get<Args>(tuple_args)...);
    }

    template<typename T>
    auto& get() noexcept
    {
        return std::get<std::vector<T>>(buffers_);
    }

    template<typename T>
    const auto& get() const noexcept
    {
        return std::get<std::vector<T>>(buffers_);
    }
};

template<typename Id, typename... Ts>
insert_buffer(const matter::unordered_typed_ids<Id, Ts...>& ids)
    ->insert_buffer<Id, Ts...>;

template<typename Id, typename... Ts, typename... Us>
insert_buffer(const matter::unordered_typed_ids<Id, Ts...>&,
              matter::insert_buffer<Id, Us...> &&)
    ->insert_buffer<Id, Ts...>;
} // namespace matter

#endif
