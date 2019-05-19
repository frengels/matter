#ifndef MATTER_COMPONENT_GROUP_HPP
#define MATTER_COMPONENT_GROUP_HPP

#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <iterator>

#include "matter/component/any_group.hpp"
#include "matter/component/component_view.hpp"
#include "matter/component/group_slice.hpp"
#include "matter/component/insert_buffer.hpp"
#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/storage/erased_storage.hpp"
#include "matter/util/container.hpp"
#include "matter/util/id_erased.hpp"

namespace matter
{
template<typename Id, typename... Cs>
class group : public matter::group_slice<Id, Cs...> {
    using base_ = matter::group_slice<Id, Cs...>;

public:
    using id_type = Id;

    using iterator = typename base_::iterator;

    template<typename _Id, typename... Ts>
    friend class group;

protected:
    explicit constexpr group(
        matter::component_storage_t<Cs>&... stores) noexcept
        : base_{stores...}
    {}

public:
    explicit constexpr group(
        matter::any_group<id_type>                         grp,
        const matter::unordered_typed_ids<id_type, Cs...>& ids) noexcept
        : base_{grp, ids}
    {}

    template<typename... Us>
    constexpr group(const matter::group<id_type, Us...>& other) noexcept
        : base_{other}
    {}

    template<typename... Us>
    constexpr group&
    operator=(const matter::group<id_type, Us...>& other) noexcept
    {
        *static_cast<base_>(this) = other;
        return *this;
    }

    /*
      constexpr iterator erase(iterator pos) noexcept
      {
          std::apply(
              [&](auto... stores) {
                  (stores.erase(pos.template get<Cs>()), ...);
              },
              stores_);
      }
    */

    template<typename... Args>
    constexpr matter::component_view<Cs...>
    emplace_back(Args&&... args) noexcept
    {
        static_assert(sizeof...(Args) == sizeof...(Cs),
                      "Must pass constructor argument for each component");
        std::apply(
            [&](auto&&... stores) {
                (matter::detail::emplace_back_ambiguous(
                     stores.get(), std::forward<Args>(args)),
                 ...);
            },
            this->stores_);

        return this->back();
    }

    void reserve(std::size_t new_capacity) noexcept
    {
        std::apply(
            [&](auto&&... stores) {
                (stores.get().reserve(new_capacity), ...);
            },
            this->stores_);
    }

    template<typename... Ts>
    constexpr std::enable_if_t<(detail::type_in_list_v<Ts, Cs...> && ...),
                               iterator>
    insert_back(const matter::insert_buffer<id_type, Ts...>& buffer) noexcept(
        (std::is_nothrow_constructible_v<
             Cs,
             decltype(*std::make_move_iterator(buffer.template begin<Cs>()))> &&
         ...))
    {
        auto pos = this->size();

        (this->template get<Cs>().insert(
             this->template get<Cs>().end(),
             std::make_move_iterator(buffer.template begin<Cs>()),
             std::make_move_iterator(buffer.template end<Cs>())),
         ...);

        return iterator{pos, this->template get<Cs>()...};
    }

    constexpr std::size_t group_size() const noexcept
    {
        return sizeof...(Cs);
    }

    template<typename _Id, typename... _Cs>
    friend constexpr std::optional<matter::group<_Id, _Cs...>>
    make_group(matter::any_group<_Id>                          grp,
               const matter::unordered_typed_ids<_Id, _Cs...>& ids) noexcept;
};

template<typename Id, typename... Cs>
constexpr std::optional<matter::group<Id, Cs...>>
make_group(matter::any_group<Id>                         grp,
           const matter::unordered_typed_ids<Id, Cs...>& ids) noexcept
{
    if (grp.group_size() != ids.size())
    {
        return std::nullopt;
    }

    auto opt_stores = grp.maybe_storage(ids);

    if (opt_stores)
    {
        return std::apply(
            [](auto&... stores) { return matter::group<Id, Cs...>{stores...}; },
            *opt_stores);
    }
    else
    {
        return std::nullopt;
    }
}

} // namespace matter

#endif
