#ifndef MATTER_COMPONENT_IDENTITY_HPP
#define MATTER_COMPONENT_IDENTITY_HPP

#pragma once

#include <type_traits>

namespace matter
{
template<typename Tag>
struct identity
{
private:
    static size_t m_next_id;

    template<typename T>
    static size_t _get() noexcept
    {
        static const size_t id = m_next_id++;
        return id;
    }

public:
    template<typename T>
    static size_t get() noexcept
    {
        return _get<typename std::remove_cv<
            typename std::remove_reference<T>::type>::type>();
    }
};

template<typename Tag>
size_t identity<Tag>::m_next_id = 0;
} // namespace matter

#endif
