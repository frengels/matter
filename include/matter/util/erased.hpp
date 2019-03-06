#ifndef MATTER_UTIL_ERASED_HPP
#define MATTER_UTIL_ERASED_HPP

#pragma once

#include <algorithm>
#include <type_traits>
#include <utility>

namespace matter
{
/// \brief allow erasing a type for generic storage
/// Upon destruction the destructor of the object will be run.
/// Unlike `std::any` no mechanism to identify the correct type is provided.
class erased final {
public:
    using deleter_type = std::add_pointer_t<void(void*)>;

private:
    void*        obj_;
    deleter_type deleter_;

public:
    template<typename T, typename... Args>
    erased(std::in_place_type_t<T>,
           Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : obj_{new T(std::forward<Args>(args)...)}, deleter_{[](void* obj) {
              auto* t = static_cast<T*>(obj);
              delete t;
          }}
    {
        static_assert(std::is_constructible_v<T, Args...>,
                      "Cannot construct T from Args...");
    }

    erased(const erased&) = delete;
    erased& operator=(const erased&) = delete;

    erased(erased&& other) noexcept
        : obj_{std::move(other.obj_)}, deleter_{std::move(other.deleter_)}
    {
        // ensure we don't delete our object
        other.obj_ = nullptr;
    }

    erased& operator=(erased&& other) noexcept
    {
        clear();
        obj_     = other.obj_;
        deleter_ = other.deleter_;

        other.obj_ = nullptr;

        return *this;
    }

    ~erased()
    {
        clear();
    }

    bool empty() const noexcept
    {
        return obj_ == nullptr;
    }

    void clear() noexcept
    {
        if (!empty())
        {
            deleter_(obj_);
            obj_ = nullptr;
        }
    }

    template<typename T>
    constexpr T& get() noexcept
    {
        return *static_cast<T*>(obj_);
    }

    template<typename T>
    constexpr const T& get() const noexcept
    {
        return *static_cast<T*>(obj_);
    }
}; // namespace matter

template<typename T, typename... Args>
erased make_erased(Args&&... args) noexcept(
    std::is_nothrow_constructible_v<T, Args...>)
{
    return erased{std::in_place_type_t<T>{}, std::forward<Args>(args)...};
}
} // namespace matter

#endif
