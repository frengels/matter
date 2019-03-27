#ifndef MATTER_COMPONENT_GROUP_VECTOR_VIEW_HPP
#define MATTER_COMPONENT_GROUP_VECTOR_VIEW_HPP

#pragma once

#include "matter/component/group_vector.hpp"
#include "matter/component/group_view.hpp"

namespace matter
{
template<typename UnorderedTypedIds>
struct group_vector_view;

template<typename Id, typename... TIds>
struct group_vector_view<matter::unordered_typed_ids<Id, TIds...>>
{
public:
    using typed_ids_type = matter::unordered_typed_ids<Id, TIds...>;
    using id_type        = typename typed_ids_type::id_type;
    using ordered_ids_type =
        decltype(matter::ordered_typed_ids{std::declval<typed_ids_type>()});

private:
    template<bool Const>
    struct iterator_
    {
    private:
        using group_vector_iterator_type =
            std::conditional_t<Const,
                               typename group_vector::const_iterator,
                               typename group_vector::iterator>;
        using group_vector_base_iterator_type = std::conditional_t<
            Const,
            typename group_vector::const_iterator::vector_iterator_type,
            typename group_vector::iterator::vector_iterator_type>;

    public:
        struct begin_tag_
        {};
        struct end_tag_
        {};

        using value_type =
            std::conditional_t<Const,
                               const group_view<typename TIds::type...>,
                               group_view<typename TIds::type...>>;
        using reference         = value_type;
        using pointer           = void;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = typename std::iterator_traits<
            group_vector_iterator_type>::difference_type;

    private:
        group_vector_iterator_type it_;
        typed_ids_type             ids_;
        ordered_ids_type           ordered_ids_;

        group_vector_base_iterator_type begin_;
        group_vector_base_iterator_type end_;

    public:
        constexpr iterator_(group_vector_iterator_type it,
                            const typed_ids_type&      ids,
                            const group_vector&        vec) noexcept
            : it_{it}, ids_{ids}, ordered_ids_{matter::ordered_typed_ids{ids_}},
              begin_{vec.begin()}, end_{vec.end()}
        {}

        constexpr iterator_(begin_tag_,
                            const typed_ids_type& ids,
                            const group_vector&   vec) noexcept
            : it_{vec.begin()}, ids_{ids},
              ordered_ids_{matter::ordered_typed_ids{ids_}},
              begin_{vec.begin()}, end_{vec.end()}
        {
            while (!is_valid())
            {
                ++it_;
            }
        }

        constexpr iterator_(begin_tag_,
                            const typed_ids_type& ids,
                            group_vector&         vec) noexcept
            : it_{vec.begin()}, ids_{ids},
              ordered_ids_{matter::ordered_typed_ids{ids_}},
              begin_{vec.begin()}, end_{vec.end()}
        {
            while (!is_valid())
            {
                ++it_;
            }
        }

        constexpr iterator_(end_tag_,
                            const typed_ids_type& ids,
                            const group_vector&   vec) noexcept
            : it_{vec.end()}, ids_{ids}, ordered_ids_{matter::ordered_typed_ids{
                                             ids_}},
              begin_{vec.begin()}, end_{vec.end()}
        {}

        constexpr iterator_(end_tag_,
                            const typed_ids_type& ids,
                            group_vector&         vec) noexcept
            : it_{vec.end()}, ids_{ids}, ordered_ids_{matter::ordered_typed_ids{
                                             ids_}},
              begin_{vec.begin()}, end_{vec.end()}
        {}

        constexpr iterator_(const iterator_<false>& it) noexcept
            : it_{it.it_}, ids_{it.ids_},
              ordered_ids_{it.ordered_ids_}, begin_{it.begin_}, end_{it.end_}
        {}

        constexpr auto group_size() const noexcept
        {
            return it_.group_size();
        }

        template<bool _Const>
        constexpr auto operator==(const iterator_<_Const>& other) const noexcept
        {
            // other checks can be omitted because they're impossible to be
            // unequal if this is equal
            return it_ == other.it_;
        }

        template<bool _Const>
        constexpr auto operator!=(const iterator_<_Const>& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr auto operator==(const group_vector_iterator_type& other) const
            noexcept
        {
            return it_ == other;
        }

        constexpr auto operator!=(const group_vector_iterator_type& other) const
            noexcept
        {
            return !(*this == other);
        }

        constexpr friend auto operator==(const group_vector_iterator_type& lhs,
                                         const iterator_& rhs) noexcept
        {
            return rhs == lhs;
        }

        constexpr friend auto operator!=(const group_vector_iterator_type& lhs,
                                         const iterator_& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        constexpr iterator_& operator++() noexcept
        {
            do
            {
                ++it_;
            } while (it_ != end_ && !is_valid());

            return *this;
        }

        constexpr iterator_& operator--() noexcept
        {
            do
            {
                --it_;
            } while (it_ != begin_ && !is_valid());

            return *this;
        }

        constexpr iterator_ operator++(int) noexcept
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr iterator_ operator--(int) noexcept
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr reference operator*() noexcept
        {
            // bind the group to pass it to the constructor
            auto grp = *it_;
            return group_view{ids_, grp};
        }

        constexpr const group_view<typename TIds::type...> operator*() const
            noexcept
        {
            auto grp = *it_;
            return group_view{ids_, grp};
        }

    private:
        constexpr auto is_valid() const noexcept
        {
            auto grp = *it_;
            return grp.contains(ordered_ids_);
        }
    };

public:
    using iterator               = iterator_<false>;
    using const_iterator         = iterator_<true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    typed_ids_type                       ids_;
    ordered_ids_type                     ordered_ids_;
    std::reference_wrapper<group_vector> group_vec_;

public:
    constexpr group_vector_view(const typed_ids_type& ids,
                                group_vector&         group_vec) noexcept
        : ids_{ids}, ordered_ids_{matter::ordered_typed_ids{ids_}},
          group_vec_{group_vec}
    {
        assert(typed_ids_type::size() <= group_vec.group_size());
    }

    constexpr iterator begin() noexcept
    {
        return iterator{typename iterator::begin_tag_{}, ids_, group_vec_};
    }

    constexpr iterator end() noexcept
    {
        return iterator{typename iterator::end_tag_{}, ids_, group_vec_};
    }

    constexpr const_iterator begin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator end() const noexcept
    {
        return end();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{cend()};
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{cbegin()};
    }
};

template<typename UnorderedTypedIds>
group_vector_view(
    const UnorderedTypedIds& ids,
    group_vector& group_vec) noexcept->group_vector_view<UnorderedTypedIds>;

} // namespace matter

#endif
