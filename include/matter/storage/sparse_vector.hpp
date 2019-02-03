#ifndef MATTER_COMPONENT_SPARSE_VECTOR_HPP
#define MATTER_COMPONENT_SPARSE_VECTOR_HPP

#pragma once

#include <cassert>
#include <iterator>
#include <vector>

namespace matter
{
template<typename T,
         typename Idx       = std::size_t,
         typename Allocator = std::allocator<T>>
class sparse_vector {
public:
    using index_type = Idx;

    using allocator_type = Allocator;

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using size_type =
        typename std::vector<value_type, allocator_type>::size_type;
    using difference_type =
        typename std::vector<value_type, allocator_type>::difference_type;

    using iterator = typename std::vector<value_type, allocator_type>::iterator;
    using const_iterator =
        typename std::vector<value_type, allocator_type>::const_iterator;

    using reverse_iterator =
        typename std::vector<value_type, allocator_type>::reverse_iterator;
    using const_reverse_iterator =
        typename std::vector<value_type,
                             allocator_type>::const_reverse_iterator;

    static constexpr index_type invalid_index =
        std::numeric_limits<index_type>::max();

private:
    std::vector<index_type> m_index{};
    std::vector<index_type> m_backref{};

    std::vector<value_type, allocator_type> m_packed{};

public:
    sparse_vector() = default;

    sparse_vector(const sparse_vector&) = default;
    sparse_vector& operator=(const sparse_vector&) = default;
    sparse_vector(sparse_vector&&) noexcept        = default;
    sparse_vector& operator=(sparse_vector&&) noexcept = default;

    iterator begin() noexcept
    {
        return m_packed.begin();
    }

    iterator end() noexcept
    {
        return m_packed.end();
    }

    const_iterator begin() const noexcept
    {
        return m_packed.begin();
    }

    const_iterator end() const noexcept
    {
        return m_packed.end();
    }

    const_iterator cbegin() const noexcept
    {
        return m_packed.cbegin();
    }

    const_iterator cend() const noexcept
    {
        return m_packed.cend();
    }

    reverse_iterator rbegin() noexcept
    {
        return m_packed.rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return m_packed.rend();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return m_packed.rbegin();
    }

    const_reverse_iterator rend() const noexcept
    {
        return m_packed.rend();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return m_packed.crbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return m_packed.crend();
    }

    index_type index_of(const_iterator it) const noexcept
    {
        auto dist = std::distance(begin(), it);
        assert(static_cast<std::size_t>(dist) < m_packed.size());

        return *(m_backref.begin() + dist);
    }

    index_type index_of(const_reverse_iterator rit) const noexcept
    {
        auto dist = std::distance(rbegin(), rit);
	assert(static_cast<std::size_t>(dist) < m_packed.size());

	return *(m_backref.rbegin() + dist);
    }

    reference operator[](index_type idx) noexcept
    {
        assert(idx < std::size(m_index));
        assert(m_index[idx] != invalid_index);
        return m_packed[m_index[idx]];
    }

    const_reference operator[](index_type idx) const noexcept
    {
        assert(idx < std::size(m_index));
        return m_packed[m_index[idx]];
    }

    bool empty() const noexcept
    {
        return m_packed.empty();
    }

    bool contains(index_type idx) const noexcept
    {
        if (idx >= std::size(m_index))
        {
            return false;
        }

        return m_index[idx] != invalid_index;
    }

    size_type size() const noexcept
    {
        return m_packed.size();
    }

    size_type max_size() const noexcept
    {
        return m_packed.max_size();
    }

    void reserve(size_type new_cap)
    {
        m_index.reserve(new_cap);
        m_packed.reserve(new_cap);
        m_backref.reserve(new_cap);
    }

    size_type capacity() const noexcept
    {
        return m_packed.capacity();
    }

    void shrink_to_fit()
    {
        m_packed.shrink_to_fit();
        m_backref.shrink_to_fit();
    }

    void clear()
    {
        m_packed.clear();
        m_index.clear();
    }

    void erase(index_type idx)
    {
        assert(contains(idx));

        auto real_idx = m_index[idx];

        auto packed_it = std::begin(m_packed);
        auto back_it   = std::begin(m_backref);
        std::advance(packed_it, real_idx);
        std::advance(back_it, real_idx);

        m_packed.erase(packed_it);
        back_it      = m_backref.erase(back_it);
        m_index[idx] = invalid_index;

        // correct all sparse references because objects were shifted
        for (; back_it != std::end(m_backref); ++back_it)
        {
            real_idx          = std::distance(std::begin(m_backref), back_it);
            m_index[*back_it] = real_idx;
        }
    }

    void erase(const_iterator pos)
    {
        auto packed_idx = std::distance(std::cbegin(m_packed), pos);
        erase(m_backref[packed_idx]);
    }

    void push_back(const index_type& idx, const T& value)
    {
        assert(!contains(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto packed_idx = std::size(m_packed);
        m_packed.push_back(value);
        m_backref.push_back(static_cast<index_type>(idx));
        m_index[idx] = static_cast<index_type>(packed_idx);
    }

    void push_back(const index_type& idx, T&& value)
    {
        assert(!contains(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto packed_idx = std::size(m_packed);
        m_packed.push_back(std::move(value));
        m_backref.push_back(static_cast<index_type>(idx));
        m_index[idx] = static_cast<index_type>(packed_idx);
    }

    template<typename... Args>
    reference emplace_back(const index_type& idx, Args&&... args)
    {
        assert(!contains(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto      packed_idx = std::size(m_packed);
        reference res = m_packed.emplace_back(std::forward<Args>(args)...);
        m_backref.push_back(static_cast<index_type>(idx));
        m_index[idx] = static_cast<index_type>(packed_idx);
        return res;
    }

    void pop_back()
    {
        assert(!empty());

        // invalidate index
        auto idx     = m_backref.back();
        m_index[idx] = invalid_index;

        // pop backreference and value
        m_backref.pop_back();
        m_packed.pop_back();
    }
};
} // namespace matter

#endif
