#ifndef MATTER_COMPONENT_GROUP_VECTOR_VIEW_HPP
#define MATTER_COMPONENT_GROUP_VECTOR_VIEW_HPP

#pragma once

#include "matter/component/group_vector.hpp"
#include "matter/component/group_view.hpp"
#include "matter/util/iterator.hpp"

namespace matter
{
template<typename Id, typename... Ts>
class group_vector_view {
public:
    using unordered_ids_type = matter::unordered_typed_ids<Id, Ts...>;
    using id_type            = Id;
    using ordered_ids_type =
        decltype(matter::ordered_typed_ids{std::declval<unordered_ids_type>()});

    struct iterator
    {
    private:
        using group_vector_iterator_type =
            matter::iterator_t<group_vector<id_type>>;

        using group_vector_base_iterator_type =
            decltype(std::declval<group_vector_iterator_type>().base());

        using group_vector_sentinel_type =
            matter::sentinel_t<group_vector<id_type>>;

    public:
        using value_type        = group_view<id_type, Ts...>;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = typename std::iterator_traits<
            group_vector_iterator_type>::difference_type;

    private:
        group_vector_iterator_type it_;

        unordered_ids_type ids_;
        ordered_ids_type   ordered_ids_;

        group_vector_base_iterator_type begin_;
        group_vector_sentinel_type      end_;

    public:
        constexpr iterator() = default;

        constexpr iterator(const unordered_ids_type&  ids,
                           const ordered_ids_type&    ordered_ids,
                           group_vector_iterator_type it,
                           group_vector_sentinel_type range_end) noexcept
            : it_{it}, ids_{ids},
              ordered_ids_{ordered_ids}, begin_{it_.base()}, end_{range_end}
        {
            if (!is_valid_end())
            {
                ++(*this);
            }
        }

        constexpr auto base() const noexcept
        {
            return it_;
        }

        constexpr const auto& ids() const noexcept
        {
            return ids_;
        }

        constexpr const auto& ordered_ids() const noexcept
        {
            return ordered_ids_;
        }

        constexpr auto group_size() const noexcept
        {
            return base().group_size();
        }

        constexpr auto operator==(const iterator& other) const noexcept
        {
            // other checks can be omitted because they're impossible to be
            // unequal if this is equal
            return base() == other.base();
        }

        constexpr auto operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr auto operator==(const group_vector_iterator_type& other) const
            noexcept
        {
            return base() == other;
        }

        constexpr auto operator!=(const group_vector_iterator_type& other) const
            noexcept
        {
            return !(*this == other);
        }

        constexpr friend auto operator==(const group_vector_iterator_type& lhs,
                                         const iterator& rhs) noexcept
        {
            return rhs == lhs;
        }

        constexpr friend auto operator!=(const group_vector_iterator_type& lhs,
                                         const iterator& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        constexpr iterator& operator++() noexcept
        {
            do
            {
                ++it_;
            } while (!is_valid_end());

            return *this;
        }

        constexpr iterator& operator--() noexcept
        {
            do
            {
                --it_;
            } while (!is_valid_begin());

            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr reference operator*() const noexcept
        {
            assert(is_valid());
            auto grp = *it_;
            return group_view{ids_, grp};
        }

        constexpr auto is_end() const noexcept
        {
            return it_ == end_;
        }

        constexpr auto is_begin() const noexcept
        {
            return it_ == begin_;
        }

        constexpr auto is_valid() const noexcept
        {
            return (*it_).contains(ordered_ids_);
        }

        constexpr auto is_valid_end() const noexcept
        {
            return is_end() || is_valid();
        }

        constexpr auto is_valid_begin() const noexcept
        {
            return is_begin() || is_valid();
        }
    };

    struct reverse_iterator
    {
    private:
        using group_vector_reverse_iterator_type =
            matter::reverse_iterator_t<group_vector<id_type>>;

        using group_vector_base_reverse_iterator_type =
            decltype(std::declval<group_vector_reverse_iterator_type>().base());

        using group_vector_reverse_sentinel_type =
            matter::reverse_sentinel_t<group_vector<id_type>>;

    public:
        using value_type        = group_view<id_type, Ts...>;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = typename std::iterator_traits<
            group_vector_reverse_iterator_type>::difference_type;

    private:
        group_vector_reverse_iterator_type it_;
        unordered_ids_type                 ids_;
        ordered_ids_type                   ordered_ids_;

        group_vector_base_reverse_iterator_type begin_;
        group_vector_reverse_sentinel_type      end_;

    public:
        constexpr reverse_iterator(
            const unordered_ids_type&          ids,
            const ordered_ids_type&            ordered_ids,
            group_vector_reverse_iterator_type it,
            group_vector_reverse_sentinel_type range_end) noexcept
            : it_{it}, ids_{ids},
              ordered_ids_{ordered_ids}, begin_{it_.base()}, end_{range_end}
        {
            if (!is_valid_end())
            {
                ++(*this);
            }
        }

        constexpr auto base() const noexcept
        {
            return it_;
        }

        constexpr auto group_size() const noexcept
        {
            return base().group_size();
        }

        constexpr auto operator==(const reverse_iterator& other) const noexcept
        {
            return base() == other.base();
        }

        constexpr auto operator!=(const reverse_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr auto
        operator==(const group_vector_reverse_iterator_type& other) const
            noexcept
        {
            return base() == other;
        }

        constexpr auto
        operator!=(const group_vector_reverse_iterator_type other) const
            noexcept
        {
            return !(*this == other);
        }

        constexpr friend auto
        operator==(const group_vector_reverse_iterator_type& lhs,
                   const reverse_iterator&                   rhs) noexcept
        {
            return rhs == lhs;
        }

        constexpr friend auto
        operator!=(const group_vector_reverse_iterator_type& lhs,
                   const reverse_iterator&                   rhs) noexcept
        {
            return !(lhs == rhs);
        }

        constexpr reverse_iterator& operator++() noexcept
        {
            do
            {
                ++it_;
            } while (!is_valid_end());

            return *this;
        }

        constexpr reverse_iterator& operator--() noexcept
        {
            do
            {
                --it_;
            } while (!is_valid_begin());

            return *this;
        }

        constexpr reverse_iterator operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr reverse_iterator operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr reference operator*() const noexcept
        {
            assert(is_valid());
            auto grp = *it_;
            return group_view{ids_, grp};
        }

        constexpr auto is_end() const noexcept
        {
            return it_ == end_;
        }

        constexpr auto is_begin() const noexcept
        {
            return it_ == begin_;
        }

        constexpr auto is_valid() const noexcept
        {
            return (*it_).contains(ordered_ids_);
        }

        constexpr auto is_valid_begin() const noexcept
        {
            return is_begin() || is_valid();
        }

        constexpr auto is_valid_end() const noexcept
        {
            return is_end() || is_valid();
        }
    };

    struct sentinel
    {
        constexpr sentinel() noexcept = default;

        constexpr auto operator==(const iterator& it) const noexcept
        {
            return it.is_end();
        }

        constexpr friend auto operator==(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return sent == it;
        }

        constexpr auto operator!=(const iterator& it) const noexcept
        {
            return !(*this == it);
        }

        constexpr friend auto operator!=(const iterator& it,
                                         const sentinel& sent) noexcept
        {
            return !(it == sent);
        }
    };

    struct reverse_sentinel
    {
        constexpr reverse_sentinel() noexcept = default;

        constexpr auto operator==(const reverse_iterator& rit) const noexcept
        {
            return rit.is_end();
        }

        constexpr friend auto operator==(const reverse_iterator& rit,
                                         const reverse_sentinel& sent) noexcept
        {
            return sent == rit;
        }

        constexpr auto operator!=(const reverse_iterator& rit) const noexcept
        {
            return !(*this == rit);
        }

        constexpr friend auto operator!=(const reverse_iterator& rit,
                                         const reverse_sentinel& sent) noexcept
        {
            return sent != rit;
        }
    };

private:
    unordered_ids_type                            ids_;
    ordered_ids_type                              ordered_ids_;
    std::reference_wrapper<group_vector<id_type>> group_vec_;

public:
    constexpr group_vector_view(const unordered_ids_type& ids,
                                group_vector<id_type>&    group_vec) noexcept
        : ids_{ids}, ordered_ids_{matter::ordered_typed_ids{ids_}},
          group_vec_{group_vec}
    {
        assert(ids.size() <= group_vec.group_size());
    }

    constexpr auto& base() noexcept
    {
        return group_vec_.get();
    }

    constexpr const auto& base() const noexcept
    {
        return group_vec_.get();
    }

    constexpr iterator begin() noexcept
    {
        return iterator{ids_,
                        ordered_ids_,
                        group_vec_.get().begin(),
                        group_vec_.get().end()};
    }

    constexpr auto end() const noexcept
    {
        return sentinel{};
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{ids_,
                                ordered_ids_,
                                group_vec_.get().rbegin(),
                                group_vec_.get().rend()};
    }

    constexpr auto rend() const noexcept
    {
        return reverse_sentinel{};
    }
};

template<typename Id, typename... Ts>
group_vector_view(const matter::unordered_typed_ids<Id, Ts...>&,
                  group_vector<Id>&)
    ->group_vector_view<Id, Ts...>;

} // namespace matter

#endif
