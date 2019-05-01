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

    constexpr const void* get() const noexcept
    {
        return value_;
    }
};

namespace detail
{
template<typename Id>
struct erased_storage_vtable
{

    using size_type = std::size_t;
    using id_type   = typename matter::id_erased<Id>::id_type;

    using get_function_type =
        std::add_pointer_t<void*(matter::erased&, size_type)>;
    using push_back_function_type =
        std::add_pointer_t<void(matter::erased&, const void*)>;
    using erase_function_type =
        std::add_pointer_t<void(matter::erased&, size_type idx)>;
    using size_function_type =
        std::add_pointer_t<std::size_t(const matter::erased&)>;
    // needed to create a new storage when it's not available beforehand
    using create_function_type =
        std::add_pointer_t<matter::id_erased<id_type>(id_type)>;

private:
    get_function_type       get_fn_;
    push_back_function_type pb_fn_;
    erase_function_type     erase_fn_;
    size_function_type      size_fn_;
    create_function_type    create_fn_;

public:
    template<typename C>
    constexpr erased_storage_vtable(std::in_place_type_t<C>) noexcept
        : get_fn_{[](matter::erased& er_storage, size_type idx) {
              auto& storage =
                  er_storage.template get<matter::component_storage_t<C>>();
              return static_cast<void*>(std::addressof(storage[idx]));
          }},
          pb_fn_{[](matter::erased& er_storage, const void* obj) {
              auto& storage =
                  er_storage.template get<matter::component_storage_t<C>>();
              const auto& comp = *static_cast<const C*>(obj);
              storage.push_back(comp);
          }},
          erase_fn_{[](matter::erased& er_storage, size_type idx) {
              auto& storage =
                  er_storage.template get<matter::component_storage_t<C>>();

              // use swap and pop to efficiently erase without mass moving
              auto last_idx = storage.size() - 1;
              storage[idx]  = std::move(storage[last_idx]);

              if constexpr (matter::has_erase_for<
                                matter::component_storage_t<C>,
                                size_type>::value)
              {
                  storage.erase(last_idx);
              }
              else
              {
                  storage.erase(std::begin(storage) + last_idx);
              }
          }},
          size_fn_{[](const matter::erased& er_storage) {
              const auto& storage =
                  er_storage.template get<matter::component_storage_t<C>>();
              return storage.size();
          }},
          create_fn_{[](id_type id) {
              return id_erased{
                  id, std::in_place_type_t<matter::component_storage_t<C>>{}};
          }}
    {
        static_assert(matter::is_component_v<C>,
                      "C does not fulfil the component contract");
    }

    /// uses the underlying operator[], no guarantees about const correctness
    constexpr void* get_at(matter::erased& er_storage, size_type idx) noexcept
    {
        return get_fn_(er_storage, idx);
    }

    constexpr void push_back(matter::erased& er_storage,
                             const void*     obj) noexcept
    {
        return pb_fn_(er_storage, obj);
    }

    constexpr void erase(matter::erased& er_storage, size_type idx) noexcept
    {
        return erase_fn_(er_storage, idx);
    }

    constexpr std::size_t size(const matter::erased& er_storage) const noexcept
    {
        return size_fn_(er_storage);
    }

    matter::id_erased<id_type> create_storage(id_type id) const noexcept
    {
        return create_fn_(id);
    }
};
} // namespace detail

template<typename Id>
struct erased_storage
{
    using id_type    = typename matter::id_erased<Id>::id_type;
    using value_type = erased_component<id_type>;
    using size_type  = std::size_t;

private:
    matter::id_erased<id_type> erased_;

    detail::erased_storage_vtable<id_type>* vptr_;

public:
    template<typename TId>
    explicit erased_storage(const TId& tid) noexcept(
        std::is_nothrow_constructible_v<
            matter::id_erased<id_type>,
            matter::component_storage_t<typename TId::id_type>,
            std::in_place_type_t<typename TId::type>>)
        : erased_{tid.value(),
                  std::in_place_type_t<
                      matter::component_storage_t<typename TId::type>>{}},
          vptr_{[]() {
              static detail::erased_storage_vtable<id_type> vobj{
                  std::in_place_type_t<typename TId::type>{}};
              return std::addressof(vobj);
          }()}
    {
        static_assert(matter::is_typed_id_v<TId>,
                      "You must use a typed id to construct erased_storage.");
    }

    erased_storage(matter::id_erased<id_type>&&            erased,
                   detail::erased_storage_vtable<id_type>* vptr) noexcept
        : erased_{std::move(erased)}, vptr_{vptr}
    {}

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
        return {erased_.id(), vptr_->get_at(erased_.base(), idx)};
    }

    constexpr void
    push_back(const erased_component<id_type>& erased_comp) noexcept
    {
        assert(erased_comp.id() == id());
        return vptr_->push_back(erased_.base(), erased_comp.get());
    }

    constexpr void erase(size_type idx) noexcept
    {
        return vptr_->erase(erased_.base(), idx);
    }

    constexpr std::size_t size() const noexcept
    {
        return vptr_->size(erased_.base());
    }

    /// create another storage of this type, the contents are not copied
    matter::erased_storage<id_type> duplicate_storage() const noexcept
    {
        return matter::erased_storage<id_type>{vptr_->create_storage(id()),
                                               vptr_};
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

        auto* tmp_vptr = lhs.vptr_;
        lhs.vptr_      = rhs.vptr_;
        rhs.vptr_      = tmp_vptr;
    }
};

template<typename TId>
erased_storage(const TId& tid)->erased_storage<typename TId::id_type>;
} // namespace matter

#endif
