#pragma once

#include <array>
#include <type_traits>

namespace matter
{
template<typename T>
class span {
public:
    using element_type           = T;
    using value_type             = std::remove_cv_t<T>;
    using index_type             = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using iterator               = T*;
    using const_iterator         = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    T* first_{nullptr};
    T* last_{nullptr};

public:
    span() = default;

    constexpr span(pointer ptr, index_type count) noexcept
        : first_{ptr}, last_{ptr + count}
    {}

    constexpr span(pointer first, pointer last) noexcept
        : first_{first}, last_{last}
    {}

    template<std::size_t N>
    constexpr span(element_type (&arr)[N]) noexcept
        : first_{arr + 0}, last_{arr + N}
    {}

    template<std::size_t N>
    constexpr span(std::array<value_type, N>& arr) noexcept
        : first_{std::data(arr)}, last_{std::data(arr) + N}
    {}

    template<std::size_t N>
    constexpr span(const std::array<value_type, N>& arr) noexcept
        : first_{std::data(arr)}, last_{std::data(arr) + N}
    {}

    template<typename Container>
    constexpr span(Container& cont)
        : span{std::data(cont), std::data(cont) + std::size(cont)}
    {}

    template<typename Container>
    constexpr span(const Container& cont)
        : span{std::data(cont), std::data(cont) + std::size(cont)}
    {}

    constexpr iterator begin() const noexcept
    {
        return first_;
    }

    constexpr iterator end() const noexcept
    {
        return last_;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return first_;
    }

    constexpr const_iterator cend() const noexcept
    {
        return last_;
    }

    constexpr reverse_iterator rbegin() const noexcept
    {
        return std::reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() const noexcept
    {
        return std::reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return std::reverse_iterator{cend()};
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return std::reverse_iterator{cbegin()};
    }

    friend constexpr iterator begin(span s) noexcept
    {
        return s.begin();
    }

    friend constexpr iterator end(span s) noexcept
    {
        return s.end();
    }

    constexpr reference front() const noexcept
    {
        return *begin();
    }

    constexpr reference back() const noexcept
    {
        return *(end() - 1);
    }

    constexpr reference operator[](index_type idx) const noexcept
    {
        return *(begin() + idx);
    }

    constexpr pointer data() const noexcept
    {
        return first_;
    }

    constexpr index_type size() const noexcept
    {
        return last_ - first_;
    }

    constexpr index_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    constexpr bool empty() const noexcept
    {
        return first_ == last_;
    }

    constexpr span<element_type> first(std::size_t count) const noexcept
    {
        return span{first_, first_ + count};
    }

    constexpr span<element_type> last(std::size_t count) const noexcept
    {
        return span{last_ - count, last_};
    }

    constexpr span<element_type> subspan(std::size_t offset) const noexcept
    {
        return span{first_ + offset, last_};
    }

    constexpr span<element_type> subspan(std::size_t offset,
                                         std::size_t count) const noexcept
    {
        return span{first_ + offset, first_ + offset + count};
    }
};

template<typename T, std::size_t N>
span(std::array<T, N>&)->span<T>;

template<typename T, std::size_t N>
span(const std::array<T, N>&)->span<const T>;
} // namespace matter