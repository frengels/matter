#ifndef MATTER_COMPONENT_RANGE_HPP
#define MATTER_COMPONENT_RANGE_HPP

#pragma once

#include <iterator>
#include <tuple>

#include "matter/component/traits.hpp"

namespace matter
{
template<typename... Cs>
struct range
{
private:
    struct begin_tag
    {};
    struct end_tag
    {};

public:
    struct iterator
    {
    public:
        using value_type       = std::tuple<Cs...>;
        using reference        = std::tuple<Cs&...>;
        using pointer          = std::tuple<Cs*...>;
        using iterator_categor = std::bidirectional_iterator_tag;
        using difference_type  = int;

    private:
        range&      range_;
        std::size_t index_{0};
        std::size_t vec_index_{0};

    public:
        explicit iterator(begin_tag, range& r) : range_{r}
        {}

        explicit iterator(end_tag, range& r)
            : range_{r}, index_{range_.stores_.size() - 1},
              vec_index_{std::get<0>(range_.stores_[index_]).size()}
        {
            // vec_index_ one past end
        }

        bool operator==(const iterator& other) noexcept
        {
            return std::addressof(range_) == std::addressof(other.range_) &&
                   index_ == other.index_ && vec_index_ == other.vec_index_;
        }

        bool operator!=(const iterator& other) noexcept
        {
            return !(*this == other);
        }

        // preincrement
        iterator& operator++() noexcept
        {
            if (++vec_index_ == std::get<0>(range_.stores_[index_]).size() &&
                (range_.stores_.size() != index_ + 1))
            {
                // conditions are met to reset and move to the next group
                ++index_;
                vec_index_ = 0;
            }

            return *this;
        }

        iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--() noexcept
        {
            if (vec_index_ == 0 && index_ != 0)
            {
                --index_;
                vec_index_ = std::get<0>(range_.stores_[index_]).size() - 1;
            }
            else if (vec_index_ != 0)
            {
                --vec_index_;
            }

            return *this;
        }

        iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        iterator& operator+=(int movement) noexcept
        {
            if (auto grp_size = std::get<0>(range_.stores_[index_]).size();
                vec_index_ + movement >= grp_size &&
                range_.stores_.size() != index_ + 1)
            {
                auto carried = vec_index_ + movement - grp_size;
                ++index_;
                vec_index_ = carried;
            }
            else
            {
                vec_index_ += movement;
            }

            return *this;
        }

        std::tuple<Cs&...> operator*() noexcept
        {
            return {std::get<matter::component_storage_t<Cs>&>(
                range_.stores_[vec_index_])[index_]...};
        }
    };

private:
    std::vector<std::tuple<matter::component_storage_t<Cs>&...>> stores_;

public:
    range(std::vector<std::tuple<matter::component_storage_t<Cs>&...>>&& stores)
        : stores_{std::move(stores)}
    {}

    iterator begin() noexcept
    {
        return iterator(begin_tag{}, *this);
    }

    iterator end() noexcept
    {
        return iterator(end_tag{}, *this);
    }

    bool empty() const noexcept
    {
        return stores_.empty();
    }

    std::tuple<matter::component_storage_t<Cs>&...>&
    group(std::size_t i) noexcept
    {
        assert(i < stores_.size());
        return stores_[i];
    }
};
} // namespace matter

#endif
