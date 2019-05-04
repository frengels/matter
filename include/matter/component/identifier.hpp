#ifndef MATTER_COMPONENT_IDENTIFIER_HPP
#define MATTER_COMPONENT_IDENTIFIER_HPP

#pragma once

#include <atomic>
#include <type_traits>

namespace matter
{
template<typename T, typename Tag>
struct identifier
{
    using value_type = T;

private:
    static std::atomic<value_type> m_next_id;

    template<typename... Ts>
    static value_type _get() noexcept
    {
        static const std::size_t id = m_next_id++;
        return id;
    }

public:
    template<typename... Ts>
    static value_type get() noexcept
    {
        return _get<typename std::remove_cv<
            typename std::remove_reference<Ts>::type>::type...>();
    }
};

template<typename T, typename Tag>
std::atomic<T> identifier<T, Tag>::m_next_id = 0;
} // namespace matter

#endif
