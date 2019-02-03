#ifndef MATTER_STORAGE_TRAITS_HPP
#define MATTER_STORAGE_TRAITS_HPP

#pragma once

#include <type_traits>

#include "matter/util/meta.hpp"

namespace matter
{
namespace detail
{
template<typename Storage, typename = void>
struct storage_types
{};

template<typename Storage>
struct storage_types<
    Storage,
    std::void_t<
        typename Storage::id_type,
        typename Storage::value_type,
        typename Storage::iterator,
        typename Storage::const_iterator,
        typename Storage::reverse_iterator,
        typename Storage::const_reverse_iterator,
        decltype(Storage()),
        matter::detail::enable_if_same_t<
            void,
            decltype(std::declval<Storage&>().emplace(
                std::declval<typename Storage::id_type>(),
                std::declval<typename Storage::value_type&&>()))>,
        matter::detail::enable_if_same_t<
            void,
            decltype(std::declval<Storage&>().erase(
                std::declval<typename Storage::id_type>()))>,
        matter::detail::enable_if_same_t<
            bool,
            decltype(std::declval<const Storage&>().contains(
                std::declval<typename Storage::id_type>()))>,
        matter::detail::enable_if_same_t<typename Storage::iterator,
                                         decltype(
                                             std::declval<Storage&>().begin())>,
        matter::detail::enable_if_same_t<
            typename Storage::const_iterator,
            decltype(std::declval<const Storage&>().begin())>,
        matter::detail::enable_if_same_t<typename Storage::iterator,
                                         decltype(
                                             std::declval<Storage&>().end())>,
        matter::detail::enable_if_same_t<
            typename Storage::const_iterator,
            decltype(std::declval<const Storage&>().end())>,
        matter::detail::enable_if_same_t<
            typename Storage::reverse_iterator,
            decltype(std::declval<Storage&>().rbegin())>,
        matter::detail::enable_if_same_t<
            typename Storage::const_reverse_iterator,
            decltype(std::declval<const Storage&>().rbegin())>,
        matter::detail::enable_if_same_t<typename Storage::reverse_iterator,
                                         decltype(
                                             std::declval<Storage&>().rend())>,
        matter::detail::enable_if_same_t<
            typename Storage::const_reverse_iterator,
            decltype(std::declval<const Storage&>().rend())>,
        matter::detail::enable_if_same_t<
            typename Storage::id_type,
            decltype(std::declval<const Storage&>().index_of(
                std::declval<typename Storage::const_iterator>()))>,
        matter::detail::enable_if_same_t<
            typename Storage::id_type,
            decltype(std::declval<const Storage&>().index_of(
                std::declval<typename Storage::const_reverse_iterator>()))>,
        matter::detail::enable_if_same_t<
            typename Storage::value_type&,
            decltype(std::declval<
                     Storage&>()[std::declval<typename Storage::id_type>()])>,
        matter::detail::enable_if_same_t<
            const typename Storage::value_type&,
            decltype(std::declval<const Storage&>()
                         [std::declval<typename Storage::id_type>()])>>>
{
    using id_type                = typename Storage::id_type;
    using value_type             = typename Storage::value_type;
    using iterator               = typename Storage::iterator;
    using const_iterator         = typename Storage::const_iterator;
    using reverse_iterator       = typename Storage::reverse_iterator;
    using const_reverse_iterator = typename Storage::const_reverse_iterator;

    static Storage
    make_storage() noexcept(std::is_nothrow_constructible_v<Storage>)
    {
        return Storage();
    }

    template<typename... Args>
    static void emplace(Storage& store, id_type id, Args&&... args) noexcept(
        noexcept(store.emplace(id, std::forward<Args>(args)...)))
    {
        static_assert(std::is_constructible_v<value_type, Args...>,
                      "cannot emplace value_type from Args into Storage");

        store.emplace(id, std::forward<Args>(args)...);
    }

    static void erase(Storage& store,
                      id_type  id) noexcept(noexcept(store.erase(id)))
    {
        store.erase(id);
    }

    static bool contains(const Storage& store,
                         id_type id) noexcept(noexcept(store.contains(id)))
    {
        return store.contains(id);
    }

    static iterator begin(Storage& store) noexcept(noexcept(store.begin()))
    {
        return store.begin();
    }

    static const_iterator
    begin(const Storage& store) noexcept(noexcept(store.begin()))
    {
        return store.begin();
    }

    static iterator end(Storage& store) noexcept(noexcept(store.end()))
    {
        return store.end();
    }

    static const_iterator end(const Storage& store) noexcept(noexcept(store.end()))
    {
        return store.end();
    }

    static reverse_iterator
    rbegin(Storage& store) noexcept(noexcept(store.rbegin()))
    {
        return store.rbegin();
    }

    static const_reverse_iterator
    rbegin(const Storage& store) noexcept(noexcept(store.rbegin()))
    {
        return store.rbegin();
    }

    static reverse_iterator
    rend(Storage& store) noexcept(noexcept(store.rend()))
    {
        return store.rend();
    }

    static const_reverse_iterator rend(const Storage& store) noexcept(
        noexcept(store.rend()))
    {
        return store.rend();
    }

    static id_type
    index_of(const Storage& store,
             const_iterator it) noexcept(noexcept(store.index_of(it)))
    {
        return store.index_of(it);
    }

    static id_type
    index_of(const Storage&         store,
             const_reverse_iterator rit) noexcept(noexcept(store.index_of(rit)))
    {
        return store.index_of(rit);
    }

    static value_type& get(Storage& store,
                           id_type  id) noexcept(noexcept(store[id]))
    {
        return store[id];
    }

    static const value_type& get(const Storage& store,
                                 id_type id) noexcept(noexcept(store[id]))
    {
        return store[id];
    }
};
} // namespace detail

template<typename Storage>
struct storage_traits : detail::storage_types<Storage>
{};
} // namespace matter

#endif
