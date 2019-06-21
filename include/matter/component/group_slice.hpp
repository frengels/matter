#ifndef MATTER_COMPONENT_SUBGROUP_HPP
#define MATTER_COMPONENT_SUBGROUP_HPP

#pragma once

#include "matter/component/any_group.hpp"
#include "matter/component/component_view.hpp"
#include "matter/component/traits.hpp"
#include "matter/id/typed_id.hpp"

namespace matter
{
/// extracts a subset of a group.
/// The passed types can be marked as const to be read only.
/// implements the entire interface required by the storage concept.
template<typename Id, typename... Ts>
class group_slice {
    static_assert(sizeof...(Ts) >= 1, "Need at least one component storage");

    template<typename _Id, typename... Us>
    friend class group_slice;

public:
    using id_type = Id;

    /// This is an iterator for the subgroup.
    /// Unlike regular iterators this uses a reference to the storage and index
    /// to iterate over the stores. The reason for this is that this makes it
    /// easier to implement some kinds of stores such as a pool with chunks that
    /// might otherwise have to check for the end of the chunk on each
    /// increment.
    /// Stores should mostly be contiguous data structures so index iteration
    /// should in all cases be available and be ass efficient if not more
    /// efficient than iterator based iteration. Another major benefit is that
    /// on reallocation of the underlying data structure these iterators don't
    /// get invalidated.
    class iterator {
    public:
        using value_type        = matter::component_view<Ts...>;
        using reference         = value_type;
        using pointer           = void;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

    private:
        std::size_t idx_;
        // must be pointer to make the type default constructible
        // assume a precondition on each method for these to be non null.
        std::tuple<matter::component_storage_t<Ts>*...> stores_;

    public:
        constexpr iterator() noexcept : idx_{0}, stores_{nullptr}
        {}

        constexpr iterator(std::size_t index,
                           matter::component_storage_t<Ts>&... stores) noexcept
            : idx_{index}, stores_{std::addressof(stores)...}
        {}

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return idx_ == other.idx_ && stores_ == other.stores_;
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr bool operator<(const iterator& other) const noexcept
        {
            return idx_ < other.idx_;
        }

        constexpr bool operator>(const iterator& other) const noexcept
        {
            return idx_ > other.idx_;
        }

        constexpr bool operator<=(const iterator& other) const noexcept
        {
            return idx_ <= other.idx_;
        }

        constexpr bool operator>=(const iterator& other) const noexcept
        {
            return idx_ >= other.idx_;
        }

        constexpr iterator& operator++() noexcept
        {
            assert(not_null());
            ++idx_;
            return *this;
        }

        constexpr iterator& operator--() noexcept
        {
            assert(not_null());
            --idx_;
            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            assert(not_null());
            auto ret = *this;
            ++(*this);
            return ret;
        }

        constexpr iterator operator--(int) noexcept
        {
            assert(not_null());
            auto ret = *this;
            --(*this);
            return ret;
        }

        constexpr iterator& operator+=(difference_type movement) noexcept
        {
            assert(not_null());
            idx_ += movement;
            return *this;
        }

        constexpr iterator& operator-=(difference_type movement) noexcept
        {
            assert(not_null());
            idx_ -= movement;
            return *this;
        }

        constexpr iterator operator+(difference_type movement) const noexcept
        {
            assert(not_null());
            auto ret = *this;
            ret += movement;
            return ret;
        }

        constexpr iterator operator-(difference_type movement) const noexcept
        {
            assert(not_null());
            auto ret = *this;
            ret -= movement;
            return ret;
        }

        constexpr difference_type operator-(const iterator& it) const noexcept
        {
            assert(not_null());
            return idx_ - it.idx_;
        }

        constexpr reference operator*() const noexcept
        {
            assert(not_null());
            return std::apply(
                [&](auto*... stores) { return reference{(*stores)[idx_]...}; },
                stores_);
        }

    private:
        constexpr bool not_null() const noexcept
        {
            return std::apply([](auto*... stores) { return (stores && ...); },
                              stores_);
        }
    };

protected:
    std::tuple<std::reference_wrapper<matter::component_storage_t<Ts>>...>
        stores_;

protected:
    constexpr group_slice(matter::component_storage_t<Ts>&... stores) noexcept
        : stores_{stores...}
    {}

public:
    constexpr group_slice(
        matter::any_group<Id>                         grp,
        const matter::unordered_typed_ids<Id, Ts...>& ids) noexcept
        : group_slice{grp.storage(ids.template get<Ts>())...}
    {
        assert(grp.group_size() <= ids.size());
        assert(grp.contains(matter::ordered_typed_ids{ids}));
    }

    template<typename... Us>
    constexpr group_slice(
        const matter::group_slice<id_type, Us...>& other) noexcept
        : stores_{
              std::get<std::reference_wrapper<matter::component_storage_t<Ts>>>(
                  other.stores_)...}
    {
        static_assert(sizeof...(Ts) == sizeof...(Us),
                      "Subgroups must be of equal size for conversion");
        static_assert((matter::detail::type_in_list_v<Us, Ts...> && ...),
                      "Incompatible types, cannot construct");
    }

    template<typename... Us>
    constexpr std::enable_if_t<sizeof...(Ts) == sizeof...(Us) &&
                                   (matter::detail::type_in_list_v<Us, Ts...> &&
                                    ...),
                               group_slice&>
    operator=(const matter::group_slice<id_type, Us...>& other) noexcept
    {
        // use lambda to workaround the fold expression limitation
        auto assign = [](auto& lhs, auto& rhs) { lhs = rhs; };

        // reassign all reference wrappers
        (assign(
             std::get<std::reference_wrapper<matter::component_storage_t<Ts>>>(
                 stores_),
             std::get<std::reference_wrapper<matter::component_storage_t<Ts>>>(
                 other.stores_)),
         ...);

        return *this;
    }

    template<typename... Us>
    constexpr std::enable_if_t<sizeof...(Ts) == sizeof...(Us) &&
                                   (matter::detail::type_in_list_v<Us, Ts...> &&
                                    ...),
                               bool>
    operator==(const matter::group_slice<id_type, Us...>& other) const noexcept
    {
        // use a lambda to circumvent the limitations of fold expressions
        auto comp = [](const auto& lhs, const auto& rhs) { return lhs == rhs; };
        // check the address of each storage as they never get relocated in
        // memory and therefore satisfies equality
        return (
            comp(std::addressof(std::get<std::reference_wrapper<
                                    matter::component_storage_t<Ts>>>(stores_)
                                    .get()),
                 std::addressof(
                     std::get<std::reference_wrapper<
                         matter::component_storage_t<Ts>>>(other.stores_)
                         .get())) &&
            ...);
    }

    template<typename... Us>
    constexpr std::enable_if_t<sizeof...(Ts) == sizeof...(Us) &&
                                   (matter::detail::type_in_list_v<Us, Ts...> &&
                                    ...),
                               bool>
    operator!=(const matter::group_slice<id_type, Us...>& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr iterator begin() noexcept
    {
        return std::apply(
            [](auto&&... stores) {
                return iterator{0, stores.get()...};
            },
            stores_);
    }

    constexpr iterator end() noexcept
    {
        return std::apply(
            [&](auto&&... stores) {
                return iterator{size(), stores.get()...};
            },
            stores_);
    }

    constexpr std::size_t size() const noexcept
    {
        return std::size(std::get<0>(stores_).get());
    }

    constexpr bool empty() const noexcept
    {
        return std::empty(std::get<0>(stores_).get());
    }

    template<std::size_t N>
    constexpr std::size_t capacity() const noexcept
    {
        return get<N>().capacity();
    }

    template<typename T>
    constexpr std::size_t capacity() const noexcept
    {
        return get<T>().capacity();
    }

    constexpr matter::component_view<Ts...>
    operator[](std::size_t index) noexcept
    {
        return std::apply(
            [&](auto&&... stores) {
                return matter::component_view<Ts...>{stores.get()[index]...};
            },
            stores_);
    }

    constexpr matter::component_view<std::add_const_t<Ts>...>
    operator[](std::size_t index) const noexcept
    {
        return std::apply(
            [&](auto&&... stores) {
                return matter::component_view<std::add_const_t<Ts>...>{
                    stores.get()[index]...};
            },
            stores_);
    }

    constexpr matter::component_view<Ts...> back() noexcept
    {
        return std::apply(
            [](auto&&... stores) {
                return matter::component_view<Ts...>{stores.get().back()...};
            },
            stores_);
    }

    constexpr matter::component_view<std::add_const_t<Ts>...> back() const
        noexcept
    {
        return std::apply(
            [](auto&&... stores) {
                return matter::component_view<std::add_const_t<Ts>...>{
                    stores.get().back()...};
            },
            stores_);
    }

    template<typename T>
    constexpr bool contains() const noexcept
    {
        return matter::detail::type_in_list_v<T, Ts...>;
    }

    template<typename T>
    constexpr bool contains(const matter::typed_id<id_type, T>&) const noexcept
    {
        return contains<T>();
    }

    template<typename... Us>
    constexpr bool
    contains(const matter::unordered_typed_ids<id_type, Us...>&) const noexcept
    {
        return (contains<Us>() && ...);
    }

    template<typename... Us>
    constexpr bool
    contains(const matter::ordered_typed_ids<id_type, Us...>&) const noexcept
    {
        return (contains<Us>() && ...);
    }

protected:
    template<std::size_t N>
    constexpr auto& get() noexcept
    {
        return std::get<N>(stores_).get();
    }

    template<std::size_t N>
    constexpr const auto& get() const noexcept
    {
        return std::get<N>(stores_).get();
    }

    template<typename T>
    constexpr auto& get() noexcept
    {
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }

    template<typename T>
    constexpr const auto& get() const noexcept
    {
        return std::get<std::reference_wrapper<matter::component_storage_t<T>>>(
                   stores_)
            .get();
    }

public:
    template<typename _Id, typename... _Ts>
    friend constexpr std::optional<matter::group_slice<_Id, _Ts...>>
    make_group_slice(
        matter::any_group<_Id>                          grp,
        const matter::unordered_typed_ids<_Id, _Ts...>& ids) noexcept;
};

template<typename Id, typename... Ts>
constexpr std::optional<matter::group_slice<Id, Ts...>>
make_group_slice(matter::any_group<Id>                         grp,
                 const matter::unordered_typed_ids<Id, Ts...>& ids) noexcept
{
    if (ids.size() > grp.group_size())
    {
        return std::nullopt;
    }

    auto opt_stores = grp.maybe_storage(ids);

    if (opt_stores)
    {
        return std::apply(
            [](auto&... stores) {
                return matter::group_slice<Id, Ts...>{stores...};
            },
            *opt_stores);
    }
    else
    {
        return std::nullopt;
    }
}
} // namespace matter

#endif
