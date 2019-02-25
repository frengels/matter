#ifndef MATTER_COMPONENT_IDENTIFIER_HPP
#define MATTER_COMPONENT_IDENTIFIER_HPP

#pragma once

#include <atomic>
#include <type_traits>

namespace matter
{
template<typename Tag>
struct identifier
{
private:
    static std::atomic<std::size_t> m_next_id;

    template<typename... Ts>
    static std::size_t _get() noexcept
    {
        static const std::size_t id = m_next_id++;
        return id;
    }

public:
    template<typename... Ts>
    static std::size_t get() noexcept
    {
        return _get<typename std::remove_cv<
            typename std::remove_reference<Ts>::type>::type...>();
    }
};

template<typename Tag>
std::atomic<std::size_t> identifier<Tag>::m_next_id = 0;
} // namespace matter

#endif
