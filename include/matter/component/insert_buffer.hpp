#ifndef MATTER_COMPONENT_INSERT_BUFFER_HPP
#define MATTER_COMPONENT_INSERT_BUFFER_HPP

#pragma once

#include <vector>

#include "matter/component/typed_id.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename UnorderedTypedIds>
struct insert_buffer;

template<typename Id, typename... TIds>
struct insert_buffer<matter::unordered_typed_ids<Id, TIds...>>
{
    using id_type = Id;

private:
    template<typename UnorderedTypedIds>
    friend struct insert_buffer;

private:
    matter::unordered_typed_ids<Id, TIds...>        ids_;
    std::tuple<std::vector<typename TIds::type>...> buffers_;

public:
    insert_buffer(const matter::unordered_typed_ids<Id, TIds...>& ids) noexcept
        : ids_{ids}, buffers_{}
    {}

    template<typename... UTIds>
    insert_buffer(
        const matter::unordered_typed_ids<Id, TIds...>& ids,
        matter::insert_buffer<matter::unordered_typed_ids<Id, UTIds...>>&&
            move_from) noexcept
        : ids_{ids}, buffers_{[&]() {
              // this will take the vector from the passed buffer and use any
              // buffer that we also have in the current buffer
              if constexpr (detail::type_in_list_v<typename TIds::type,
                                                   typename UTIds::type...>)
              {
                  auto vec = std::vector<typename TIds::type>{
                      std::move(move_from.template get<typename TIds::type>())};
                  vec.clear();
                  return vec;
              }
              else
              {
                  return std::vector<typename TIds::type>{};
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
        (std::is_nothrow_default_constructible_v<typename TIds::type> && ...))
    {
        return std::apply(
            [&](auto&&... buffers) { (buffers.resize(new_size), ...); },
            buffers_);
    }

    void resize(std::size_t new_size, const typename TIds::type... ts) noexcept(
        (std::is_nothrow_copy_constructible_v<typename TIds::type> && ...))
    {
        (get<typename TIds::type>().resize(new_size, ts), ...);
    }

    template<typename... TupArgs>
    std::enable_if_t<(
        detail::is_constructible_expand_tuple_v<typename TIds::type, TupArgs> &&
        ...)>
    emplace_back(TupArgs&&... tuple_args) noexcept(
        (detail::is_nothrow_constructible_expand_tuple_v<typename TIds::type,
                                                         TupArgs> &&
         ...))
    {
        (emplace_one_impl<typename TIds::type>(
             std::forward<TupArgs>(tuple_args)),
         ...);
    }

    template<typename... Args>
    std::enable_if_t<sizeof...(Args) == sizeof...(TIds) &&
                     (std::is_constructible_v<typename TIds::type, Args> &&
                      ...)>
    emplace_back(Args&&... args) noexcept(
        (std::is_nothrow_constructible_v<typename TIds::type, Args> && ...))
    {
        (get<typename TIds::type>().emplace_back(std::forward<Args>(args)),
         ...);
    }

    template<typename... Ts>
    std::enable_if_t<sizeof...(Ts) == sizeof...(TIds) &&
                     (std::is_constructible_v<typename TIds::type, const Ts&> &&
                      ...)>
    push_back(const Ts&... ts) noexcept((
        std::is_nothrow_constructible_v<typename TIds::type, const Ts&> && ...))
    {
        (get<typename TIds::type>().push_back(ts), ...);
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

template<typename UnorderedIdsType>
insert_buffer(
    const UnorderedIdsType& ids) noexcept->insert_buffer<UnorderedIdsType>;

template<typename UnorderedTypedIds, typename OtherUnorderedTypedIds>
insert_buffer(const UnorderedTypedIds& ids,
              matter::insert_buffer<OtherUnorderedTypedIds>&&
                  move_from) noexcept->insert_buffer<UnorderedTypedIds>;

} // namespace matter

#endif
