#ifndef MATTER_COMPONENT_SPARSE_VECTOR_HPP
#define MATTER_COMPONENT_SPARSE_VECTOR_HPP

#pragma once

#include <cassert>
#include <iterator>
#include <tuple>
#include <vector>

namespace matter
{
template<typename T, typename Idx = std::size_t>
struct sparse_vector
{

public:
    using index_type = Idx;

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

private:
    using value_index_pair       = std::pair<value_type, index_type>;
    using const_value_index_pair = const std::pair<value_type, index_type>;

public:
    using packed_vector_type = std::vector<value_index_pair>;

    using size_type       = typename packed_vector_type::size_type;
    using difference_type = typename packed_vector_type::difference_type;

    static constexpr index_type invalid_index =
        std::numeric_limits<index_type>::max();

    struct const_iterator;

    struct iterator
    {
    public:
        using value_type        = T;
        using difference_type   = index_type;
        using pointer           = pointer;
        using reference         = reference;
        using iterator_category = std::random_access_iterator_tag;

        friend struct const_iterator;
        friend struct sparse_vector<T>;

    private:
        value_index_pair* m_ptr{nullptr};

    public:
        constexpr iterator() = default;
        constexpr iterator(value_index_pair* ptr) : m_ptr{ptr}
        {}
        constexpr iterator(const typename packed_vector_type::iterator& it)
            : m_ptr{std::addressof(*it)}
        {}

        constexpr iterator(const iterator&) noexcept = default;
        constexpr iterator& operator=(const iterator&) noexcept = default;
        constexpr iterator(iterator&&) noexcept                 = default;
        constexpr iterator& operator=(iterator&&) noexcept = default;

        auto index() const noexcept -> decltype(m_ptr->second)
        {
            return m_ptr->second;
        }

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return m_ptr == other.m_ptr;
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr bool operator<(const iterator& other) const noexcept
        {
            return m_ptr < other.m_ptr;
        }

        constexpr bool operator<=(const iterator& other) const noexcept
        {
            return m_ptr <= other.m_ptr;
        }

        constexpr bool operator>(const iterator& other) const noexcept
        {
            return m_ptr > other.m_ptr;
        }

        constexpr bool operator>=(const iterator& other) const noexcept
        {
            return m_ptr >= other.m_ptr;
        }

        constexpr iterator& operator++() noexcept
        {
            ++m_ptr;

            return *this;
        }

        constexpr iterator operator++(int) noexcept
        {
            auto it = iterator(*this);
            ++m_ptr;

            return it;
        }

        constexpr iterator& operator--() noexcept
        {
            --m_ptr;

            return *this;
        }

        constexpr iterator operator--(int) noexcept
        {
            auto it = iterator(*this);
            --m_ptr;

            return it;
        }

        constexpr iterator& operator+=(difference_type n) noexcept
        {
            m_ptr += n;
            return *this;
        }

        constexpr iterator& operator-=(difference_type n) noexcept
        {
            m_ptr -= n;
            return *this;
        }

        constexpr iterator operator+(difference_type n) const noexcept
        {
            auto it = iterator(*this);
            it += n;
            return it;
        }

        constexpr iterator operator-(difference_type n) const noexcept
        {
            auto it = iterator(*this);
            it -= n;
            return it;
        }

        constexpr difference_type operator-(iterator rhs) const noexcept
        {
            return static_cast<difference_type>(m_ptr - rhs.m_ptr);
        }

        constexpr reference operator*() noexcept
        {
            return m_ptr->first;
        }

        constexpr const value_type& operator*() const noexcept
        {
            return m_ptr->first;
        }

        constexpr pointer operator->() noexcept
        {
            return std::addressof(m_ptr->first);
        }

        constexpr const pointer operator->() const noexcept
        {
            return std::addressof(m_ptr->first);
        }

        constexpr reference operator[](difference_type offset) noexcept
        {
            auto* ptr = m_ptr + offset;
            return ptr->first;
        }

        constexpr const value_type& operator[](difference_type offset) const
            noexcept
        {
            auto* ptr = m_ptr + offset;
            return ptr->first;
        }
    };

    struct const_iterator
    {
    public:
        using value_type        = const value_type;
        using difference_type   = index_type;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::random_access_iterator_tag;

        friend struct sparse_vector<T>;

    private:
        const value_index_pair* m_ptr{nullptr};

    public:
        constexpr const_iterator() = default;
        constexpr const_iterator(iterator it) : m_ptr{it.m_ptr}
        {}
        constexpr const_iterator(value_index_pair* ptr) : m_ptr{ptr}
        {}
        constexpr const_iterator(
            const typename packed_vector_type::const_iterator& it)
            : m_ptr{std::addressof(*it)}
        {}

        constexpr const_iterator(const const_iterator&) noexcept = default;
        constexpr const_iterator&
        operator=(const const_iterator&) noexcept           = default;
        constexpr const_iterator(const_iterator&&) noexcept = default;
        constexpr const_iterator&
        operator=(const_iterator&&) noexcept = default;

        auto index() const noexcept -> decltype(m_ptr->second)
        {
            return m_ptr->second;
        }

        constexpr bool operator==(const const_iterator& other) const noexcept
        {
            return m_ptr == other.m_ptr;
        }

        constexpr bool operator!=(const const_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr bool operator<(const const_iterator& other) const noexcept
        {
            return m_ptr < other.m_ptr;
        }

        constexpr bool operator<=(const const_iterator& other) const noexcept
        {
            return m_ptr <= other.m_ptr;
        }

        constexpr bool operator>(const const_iterator& other) const noexcept
        {
            return m_ptr > other.m_ptr;
        }

        constexpr bool operator>=(const const_iterator& other) const noexcept
        {
            return m_ptr >= other.m_ptr;
        }

        constexpr const_iterator& operator++() noexcept
        {
            ++m_ptr;

            return *this;
        }

        constexpr const_iterator operator++(int) noexcept
        {
            auto it = const_iterator(*this);
            ++m_ptr;

            return it;
        }

        constexpr const_iterator& operator--() noexcept
        {
            --m_ptr;

            return *this;
        }

        constexpr const_iterator operator--(int) noexcept
        {
            auto it = const_iterator(*this);
            --m_ptr;

            return it;
        }

        constexpr const_iterator& operator+=(difference_type n) noexcept
        {
            m_ptr += n;
            return *this;
        }

        constexpr const_iterator& operator-=(difference_type n) noexcept
        {
            m_ptr -= n;
            return *this;
        }

        constexpr const_iterator operator+(difference_type n) const noexcept
        {
            auto it = const_iterator(*this);
            it += n;
            return it;
        }

        constexpr const_iterator operator-(difference_type n) const noexcept
        {
            auto it = const_iterator(*this);
            it -= n;
            return it;
        }

        constexpr difference_type operator-(const_iterator rhs) const noexcept
        {
            return static_cast<difference_type>(m_ptr - rhs.m_ptr);
        }

        constexpr reference operator*() const noexcept
        {
            return m_ptr->first;
        }

        constexpr pointer operator->() const noexcept
        {
            return std::addressof(m_ptr->first);
        }

        constexpr reference operator[](difference_type offset) const noexcept
        {
            auto* ptr = m_ptr + offset;
            return ptr->first;
        }
    };

private:
    std::vector<index_type> m_index;

    packed_vector_type m_packed;

public:
    sparse_vector() = default;

    sparse_vector(const sparse_vector&) = default;
    sparse_vector& operator=(const sparse_vector&) = default;
    sparse_vector(sparse_vector&&) noexcept        = default;
    sparse_vector& operator=(sparse_vector&&) noexcept = default;

    auto begin() noexcept -> decltype(iterator(std::begin(m_packed)))
    {
        return iterator(std::begin(m_packed));
    }

    auto end() noexcept -> decltype(iterator(std::end(m_packed)))
    {
        return iterator(std::end(m_packed));
    }

    auto begin() const noexcept
        -> decltype(const_iterator(std::begin(m_packed)))
    {
        return const_iterator(std::begin(m_packed));
    }

    auto end() const noexcept -> decltype(const_iterator(std::end(m_packed)))
    {
        return const_iterator(std::end(m_packed));
    }

    auto cbegin() const noexcept
        -> decltype(const_iterator(std::begin(m_packed)))
    {
        return const_iterator(std::begin(m_packed));
    }

    auto cend() const noexcept -> decltype(const_iterator(std::end(m_packed)))
    {
        return const_iterator(std::end(m_packed));
    }

    reference operator[](const index_type& idx) noexcept
    {
        assert(idx < std::size(m_index));
        assert(m_index[idx] != invalid_index);
        return m_packed[m_index[idx]].first;
    }

    const_reference operator[](const index_type& idx) const noexcept
    {
        assert(idx < std::size(m_index));
        return m_packed[m_index[idx]].first;
    }

    bool empty() const noexcept
    {
        return m_packed.empty();
    }

    bool has_value(const index_type& idx) const noexcept
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
    }

    size_type capacity() const noexcept
    {
        return m_packed.capacity();
    }

    void shrink_to_fit()
    {
        m_packed.shrink_to_fit();
    }

    void clear()
    {
        m_packed.clear();
        m_index.clear();
    }

    void swap_and_pop(iterator pos)
    {
        assert(pos < end());

        auto idx        = pos.index();
        auto packed_idx = std::distance(begin(), pos);
        // last element to swap with
        auto last = end() - 1;

        auto last_idx = last.index();

	using std::swap;

	swap(pos.m_ptr->first, last.m_ptr->first);
	swap(pos.m_ptr->second, last.m_ptr->second);

        m_index[last_idx] = packed_idx;
        m_index[idx]      = invalid_index;
        m_packed.pop_back();
    }

    void swap_and_pop(const index_type& idx)
    {
        assert(has_value(idx));

        auto real_idx = m_index[idx];

        auto packed_it = begin() + real_idx;
        swap_and_pop(packed_it);
    }

    void erase(const_iterator pos)
    {
        assert(pos < std::cend(*this));
        auto index = pos.index();

        // start iterating one after to be erased
        for (const_iterator it = pos + 1; it != std::end(*this); ++it)
        {
            // all objects are being shifted forward by one
            auto index = it.index();
            --m_index[index];
        }

        m_packed.erase(typename decltype(m_packed)::const_iterator(pos.m_ptr));
        m_index[index] = invalid_index;
    }

    void erase(const index_type& idx)
    {
        assert(has_value(idx));

        auto real_idx = m_index[idx];

        auto packed_it = cbegin() + real_idx;
        erase(packed_it);
    }

    void push_back(const index_type& idx, const T& value)
    {
        assert(!has_value(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto packed_idx = std::size(m_packed);
        m_packed.emplace_back(value, idx);
        m_index[idx] = static_cast<index_type>(packed_idx);
    }

    void push_back(const index_type& idx, T&& value)
    {
        assert(!has_value(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto packed_idx = std::size(m_packed);
        m_packed.emplace_back(std::move(value), idx);
        m_index[idx] = static_cast<index_type>(packed_idx);
    }

    template<typename... Args>
    reference emplace_back(const index_type& idx, Args&&... args)
    {
        assert(!has_value(idx));

        if (idx >= std::size(m_index))
        {
            m_index.resize(idx + 1, invalid_index);
        }

        auto  packed_idx = std::size(m_packed);
        auto& res        = m_packed.emplace_back(
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<Args>(args)...),
            std::forward_as_tuple(idx));
        m_index[idx] = static_cast<index_type>(packed_idx);
        return res.first;
    }

    void pop_back()
    {
        assert(!empty());

        erase(std::end(*this));
    }
};
} // namespace matter

#endif
