#ifndef MATTER_STORAGE_ERASED_STORAGE_HPP
#define MATTER_STORAGE_ERASED_STORAGE_HPP

#pragma once

#include "matter/component/traits.hpp"
#include "matter/component/typed_id.hpp"
#include "matter/util/id_erased.hpp"

namespace matter
{
template<typename Id>
struct erased_component
{
    using id_type = Id;

private:
    /// id stored for debugging purposes
    id_type id_;
    void*   value_;

public:
    constexpr erased_component(id_type id, void* val) : id_{id}, value_{val}
    {}

    constexpr id_type id() const noexcept
    {
        return id_;
    }

    constexpr void* get() noexcept
    {
        return value_;
    }
};

struct erased_storage
{
    using id_type    = typename matter::id_erased::id_type;
    using value_type = erased_component<id_type>;
    using size_type  = std::size_t;

    using get_function_type =
        std::add_pointer_t<void*(erased_storage& erased, size_type idx)>;
    using push_back_function_type =
        std::add_pointer_t<void(erased_storage&           erased,
                                erased_component<id_type> comp)>;
    using erase_function_type =
        std::add_pointer_t<void(erased_storage& erased, size_type idx)>;

private:
    matter::id_erased       erased_;
    get_function_type       get_fn_;
    push_back_function_type pb_fn_;
    erase_function_type     erase_fn_;

public:
    template<typename TId>
    explicit erased_storage(const TId& tid) noexcept(
        std::is_nothrow_constructible_v<
            matter::id_erased,
            matter::component_storage_t<typename TId::id_type>,
            std::in_place_type_t<typename TId::type>>)
        : erased_{tid.value(),
                  std::in_place_type_t<
                      matter::component_storage_t<typename TId::type>>{}},
          get_fn_{[](erased_storage& erased, size_type idx) -> void* {
              using component_type = typename TId::type;
              auto& storage =
                  erased.erased_
                      .get<matter::component_storage_t<component_type>>();
              return static_cast<void*>(std::addressof(storage[idx]));
          }},
          pb_fn_{[](erased_storage& erased, erased_component<id_type> er_comp) {
              using component_type = typename TId::type;
              assert(erased.id() == er_comp.id());
              auto& storage =
                  erased.erased_
                      .get<matter::component_storage_t<component_type>>();
              auto& comp = *static_cast<component_type*>(er_comp.get());
              storage.push_back(comp);
          }},
          erase_fn_{[](erased_storage& erased, size_type idx) {
              using component_type = typename TId::type;
              auto& storage =
                  erased.erased_
                      .get<matter::component_storage_t<component_type>>();
              storage.erase(std::begin(storage) + idx);
          }}
    {
        static_assert(matter::is_typed_id_v<TId>,
                      "You must use a typed id to construct erased_storage.");
    }

    template<typename C>
    constexpr matter::component_storage_t<C>& get() noexcept
    {
        return erased_.template get<matter::component_storage_t<C>>();
    }

    template<typename C>
    constexpr const matter::component_storage_t<C>& get() const noexcept
    {
        return erased_.template get<matter::component_storage_t<C>>();
    }

    constexpr id_type id() const noexcept
    {
        return erased_.id();
    }

    constexpr erased_component<id_type> operator[](size_type idx) noexcept
    {
        return erased_component{erased_.id(), get_fn_(*this, idx)};
    }

    constexpr void push_back(erased_component<id_type> erased_comp) noexcept
    {
        pb_fn_(*this, std::move(erased_comp));
    }

    constexpr void erase(size_type idx) noexcept
    {
        erase_fn_(*this, idx);
    }

    constexpr auto operator==(const erased_storage& other) const noexcept
    {
        return erased_ == other.erased_;
    }

    constexpr auto operator!=(const erased_storage& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr auto operator<(const erased_storage& other) const noexcept
    {
        return erased_ < other.erased_;
    }

    constexpr auto operator>(const erased_storage& other) const noexcept
    {
        return erased_ < other.erased_;
    }

    constexpr auto operator<=(const erased_storage& other) const noexcept
    {
        return erased_ <= other.erased_;
    }

    constexpr auto operator>=(const erased_storage& other) const noexcept
    {
        return erased_ >= other.erased_;
    }

    constexpr auto operator==(id_type id) const noexcept
    {
        return erased_ == id;
    }

    constexpr auto operator!=(id_type id) const noexcept
    {
        return !(*this == id);
    }

    constexpr auto operator<(id_type id) const noexcept
    {
        return erased_ < id;
    }

    constexpr auto operator>(id_type id) const noexcept
    {
        return erased_ > id;
    }

    constexpr auto operator<=(id_type id) const noexcept
    {
        return erased_ <= id;
    }

    constexpr auto operator>=(id_type id) const noexcept
    {
        return erased_ >= id;
    }

    friend constexpr auto operator==(id_type               id,
                                     const erased_storage& storage) noexcept
    {
        return id == storage.erased_;
    }

    friend constexpr auto operator!=(id_type               id,
                                     const erased_storage& storage) noexcept
    {
        return !(id == storage);
    }

    friend constexpr auto operator<(id_type               id,
                                    const erased_storage& storage) noexcept
    {
        return id < storage.erased_;
    }

    friend constexpr auto operator>(id_type               id,
                                    const erased_storage& storage) noexcept
    {
        return id > storage.erased_;
    }

    friend constexpr auto operator>=(id_type               id,
                                     const erased_storage& storage) noexcept
    {
        return id >= storage.erased_;
    }

    friend constexpr auto operator<=(id_type               id,
                                     const erased_storage& storage) noexcept
    {
        return id <= storage.erased_;
    }

    friend void swap(erased_storage& lhs, erased_storage& rhs) noexcept
    {
        using std::swap;
        swap(lhs.erased_, rhs.erased_);

        auto* tmp1  = lhs.get_fn_;
        lhs.get_fn_ = rhs.get_fn_;
        rhs.get_fn_ = tmp1;

        auto* tmp2 = lhs.pb_fn_;
        lhs.pb_fn_ = rhs.pb_fn_;
        rhs.pb_fn_ = tmp2;

        auto* tmp3    = lhs.erase_fn_;
        lhs.erase_fn_ = rhs.erase_fn_;
        rhs.erase_fn_ = tmp3;
    }
};
} // namespace matter

#endif
