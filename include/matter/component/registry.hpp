#ifndef MATTER_COMPONENT_REGISTRY_HPP
#define MATTER_COMPONENT_REGISTRY_HPP

#pragma once

#include <algorithm>

#include "component_identifier.hpp"

#include "matter/util/id_erased.hpp"
#include "matter/util/meta.hpp"

namespace matter
{
template<typename... Components>
class registry {
public:
    template<std::size_t I>
    class group {
    public:
        static constexpr auto num_components = I;

        using id_type = typename component_identifier<Components...>::id_type;

    private:
        std::array<matter::id_erased, num_components> storage_;

    public:
        /// \brief construct a new instance of group
        /// Constructs a group from ids + types, each forwarded as a tuple using
        /// `forward_as_tuple`. After passing the array will be sorted to ensure
        /// quick lookup for finding matching ids.
        template<typename... TupArgs>
        constexpr group(TupArgs&&... id_erased_args) noexcept
            : storage_{detail::construct_from_tuple(
                  std::in_place_type_t<matter::id_erased>{},
                  std::forward<TupArgs>(id_erased_args))...}
        {
            static_assert(sizeof...(TupArgs) <= num_components,
                          "Cannot construct with this many storages.");
            static_assert(sizeof...(TupArgs) >= num_components,
                          "Cannot construct with this few storages.");

            std::sort(storage_.begin(), storage_.end());
        }

        /// \brief checks if this group contains the passed ids
        /// Each passed id must be contained within this group, effectively the
        /// same as contains but only works on arrays with the same size as the
        /// group
        constexpr bool
        operator==(const std::array<id_type, num_components>& ids) const
            noexcept
        {
            return contains(ids);
        }

        constexpr bool
        operator!=(const std::array<id_type, num_components>& ids) const
            noexcept
        {
            return !(*this == ids);
        }

        /// \brief compares the contained ids
        /// This function can be used to sort the group in the registry's vector
        /// for this group size. However doing so is complicated and I have not
        /// started working on this so far. So these comparison functions remain
        /// mainly unused for now.
        template<std::size_t N,
                 typename = std::enable_if_t<(N <= num_components)>>
        constexpr bool operator<(const std::array<id_type, N>& ids) const
            noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            if constexpr (N == num_components)
            {
                for (std::size_t i = 0; i < storage_.size(); ++i)
                {
                    if (storage_[i].id() < ids[i])
                    {
                        return true;
                    }
                    else if (storage_[i].id() > ids[i])
                    {
                        return false;
                    }
                }

                return false;
            }
            else
            {
                auto misses = num_components - N;

                for (std::size_t i = 0, j = 0; i < storage_.size(); ++i)
                {
                    if (storage_[i].id() < ids[j])
                    {
                        // check our allowed misses left
                        if (misses == 0)
                        {
                            return true;
                        }
                        else
                        {
                            --misses;
                            continue;
                            // continue without incrementing j, checking the
                            // same array[id] next iteration
                        }
                    }
                    else if (storage_[i].id() > ids[j])
                    {
                        return false;
                    }

                    ++j;
                }

                return false;
            }
        }

        constexpr bool
        operator>(const std::array<id_type, num_components>& ids) const noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() > ids[i])
                {
                    return true;
                }
                else if (storage_[i].id() < ids[i])
                {
                    return false;
                }
            }

            return false;
        }

        constexpr bool operator<(const group& other) const noexcept
        {
            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() < other.storage_[i].id())
                {
                    return true;
                }
                else if (storage_[i].id() > other.storage_[i].id())
                {
                    return false;
                }
            }

            // when equal we get here
            return false;
        }

        constexpr bool operator>(const group& other) const noexcept
        {
            for (std::size_t i = 0; i < storage_.size(); ++i)
            {
                if (storage_[i].id() > other.storage_[i].id())
                {
                    return true;
                }
                else if (storage_[i].id() < other.storage_[i].id())
                {
                    return false;
                }
            }

            // when equal we get here
            return false;
        }

        constexpr bool contains(id_type id) const noexcept
        {
            auto index_it = find_id(id);
            return index_it != storage_.end();
        }

        template<std::size_t N,
                 typename = std::enable_if_t<(N <= num_components)>>
        constexpr bool contains(const std::array<id_type, N>& ids) const
            noexcept
        {
            assert(std::is_sorted(ids.begin(), ids.end()));

            return std::includes(
                storage_.begin(), storage_.end(), ids.begin(), ids.end());
        }

        template<typename C>
        matter::component_storage_t<C>& get(id_type id) noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->template get<matter::component_storage_t<C>>();
        }

        template<typename C>
        const matter::component_storage_t<C>& get(id_type id) const noexcept
        {
            assert(contains(id));
            auto index_it = find_id(id);
            return index_it->template get<matter::component_storage_t<C>>();
        }

    private:
        /// \brief checks whether the stores are sorted
        constexpr auto is_sorted() const noexcept
        {
            return std::is_sorted(storage_.begin(), storage_.end());
        }

        constexpr auto find_id(id_type id) noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(), storage_.end(), id);

            if (index_it->id() != id)
            {
                return storage_.end();
            }

            return index_it;
        }

        constexpr auto find_id(id_type id) const noexcept
        {
            // probably redundant as the id is never changed after construction
            assert(is_sorted());

            auto index_it =
                std::lower_bound(storage_.begin(), storage_.end(), id);

            if (index_it->id() != id)
            {
                return storage_.end();
            }

            return index_it;
        }
    };

public:
    using identifier_type = component_identifier<Components...>;
    using id_type         = typename identifier_type::id_type;

private:
    identifier_type identifier_;

public:
    constexpr registry() noexcept = default;

    template<typename C>
    constexpr id_type component_id() const noexcept
    {
        return identifier_.template id<C>();
    }

    template<typename C>
    void register_component() noexcept
    {
        identifier_.template register_type<C>();
    }
};
} // namespace matter

#endif
