#pragma once

#include <type_traits>

// copied from hera::common_reference which is copied from clang's
// common_reference. Originally by Casey Carter.
namespace matter
{
template<typename T,
         typename U,
         template<typename>
         typename TQual,
         template<typename>
         typename UQual>
struct basic_common_reference
{};

namespace detail
{
template<typename T, typename U>
using cond_res =
    decltype(false ? std::declval<T (&)()>()() : std::declval<U (&)()>()());

template<typename T, typename U, typename = void>
struct common_type3
{};

template<typename T>
using cref = std::add_lvalue_reference_t<const T>;

template<typename T, typename U>
struct common_type3<T, U, std::void_t<cond_res<cref<T>, cref<U>>>>
{
    using type = std::decay_t<cond_res<cref<T>, cref<U>>>;
};

template<typename T, typename U>
struct common_type2 : common_type3<T, U>
{};
} // namespace detail

template<typename... Ts>
struct common_type;

template<>
struct common_type<>
{};

template<typename T>
struct common_type<T> : common_type<T, T>
{};

template<typename T, typename U>
struct common_type<T, U>
    : std::conditional_t<std::is_same_v<T, std::decay_t<T>> &&
                             std::is_same_v<U, std::decay_t<U>>,
                         detail::common_type2<T, U>,
                         common_type<std::decay_t<T>, std::decay_t<U>>>
{};

template<typename... Ts>
using common_type_t = typename common_type<Ts...>::type;

template<typename T, typename U, typename V, typename... Rest>
struct common_type<T, U, V, Rest...>
    : common_type<common_type_t<T, U>, V, Rest...>
{};

namespace detail
{
template<typename T>
struct copy_cv_
{
    template<typename U>
    using fn = U;
};

template<typename T>
struct copy_cv_<const T>
{
    template<typename U>
    using fn = std::add_const_t<U>;
};

template<typename T>
struct copy_cv_<volatile T>
{
    template<typename U>
    using fn = std::add_volatile_t<U>;
};

template<typename T>
struct copy_cv_<const volatile T>
{
    template<typename U>
    using fn = std::add_cv_t<U>;
};

template<typename From, typename To>
using copy_cv = typename copy_cv_<From>::template fn<To>;

template<typename T>
struct xref
{
    template<typename U>
    using fn = copy_cv<T, U>;
};

template<typename T>
struct xref<T&>
{
    template<typename U>
    using fn = std::add_lvalue_reference_t<copy_cv<T, U>>;
};

template<typename T>
struct xref<T&&>
{
    template<typename U>
    using fn = std::add_rvalue_reference_t<copy_cv<T, U>>;
};

template<typename T,
         typename U,
         typename Result = cond_res<copy_cv<T, U>&, copy_cv<U, T>&>,
         typename        = std::enable_if_t<std::is_lvalue_reference_v<Result>>>
using ll_common_ref = Result;

template<typename T, typename U>
using rr_common_ref = std::remove_reference_t<ll_common_ref<T, U>>&&;
} // namespace detail

template<typename...>
struct common_reference
{};

template<typename T>
struct common_reference<T>
{
    using type = T;
};

namespace detail
{
template<typename T, typename U, typename = void>
struct common_reference4 : hera::common_type<T, U>
{};

template<typename T, typename U>
struct common_reference4<T, U, std::void_t<cond_res<T, U>>>
{
    using type = cond_res<T, U>;
};

template<typename T, typename U>
using basic_common_ref =
    typename basic_common_reference<std::remove_cvref_t<T>,
                                    std::remove_cvref_t<U>,
                                    xref<T>::template fn,
                                    xref<U>::template fn>::type;

template<typename T, typename U, typename = void>
struct common_reference3 : common_reference4<T, U>
{};

template<typename T, typename U>
struct common_reference3<T, U, std::void_t<basic_common_ref<T, U>>>
{
    using type = basic_common_ref<T, U>;
};

template<typename T, typename U, typename = void>
struct common_reference2 : common_reference3<T, U>
{};

template<typename T, typename U>
struct common_reference2<T&, U&, std::void_t<ll_common_ref<T, U>>>
{
    using type = ll_common_ref<T, U>;
};

template<typename T, typename U>
struct common_reference2<
    T&&,
    U&,
    std::enable_if_t<std::is_convertible_v<T&&, ll_common_ref<const T, U>>>>
{
    using type = ll_common_ref<const T, U>;
};

template<typename T, typename U>
struct common_reference2<
    T&,
    U&&,
    std::enable_if_t<std::is_convertible_v<U&&, ll_common_ref<const U, T>>>>
{
    using type = ll_common_ref<const U, T>;
};

template<typename T, typename U>
struct common_reference2<
    T&&,
    U&&,
    std::enable_if_t<std::is_convertible_v<T&&, rr_common_ref<T, U>> &&
                     std::is_convertible_v<U&&, rr_common_ref<T, U>>>>
{
    using type = rr_common_ref<T, U>;
};
} // namespace detail

template<typename... Ts>
struct common_reference;

template<typename T, typename U>
struct common_reference<T, U> : detail::common_reference2<T, U>
{};

namespace detail
{
template<typename Void, typename T, typename U, typename... Ts>
struct fold_common_reference
{};

template<typename T, typename U, typename... Ts>
struct fold_common_reference<std::void_t<typename common_reference<T, U>::type>,
                             T,
                             U,
                             Ts...>
    : common_reference<typename common_reference<T, U>::type, Ts...>
{};
} // namespace detail

template<typename T, typename U, typename V, typename... Rest>
struct common_reference<T, U, V, Rest...>
    : detail::fold_common_reference<void, T, U, V, Rest...>
{};

template<typename... Ts>
using common_reference_t = typename common_reference<Ts...>::type;
} // namespace matter