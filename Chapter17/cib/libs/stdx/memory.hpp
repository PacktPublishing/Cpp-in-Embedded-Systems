#pragma once

#include <stdx/type_traits.hpp>

#include <memory>

namespace stdx {
inline namespace v1 {
template <typename T> constexpr auto to_address(T *p) noexcept -> T * {
    static_assert(not stdx::is_function_v<T>,
                  "to_address on a function pointer is ill-formed");
    return p;
}

namespace detail::detect {
template <typename T, typename = void>
constexpr auto pointer_traits_to_address = false;

template <typename T>
constexpr auto pointer_traits_to_address<
    T, std::void_t<decltype(std::pointer_traits<T>::to_address(
           std::declval<T>()))>> = true;
} // namespace detail::detect

template <typename T> constexpr auto to_address(T const &t) {
    if constexpr (detail::detect::pointer_traits_to_address<T>) {
        return std::pointer_traits<T>::to_address(t);
    } else {
        return stdx::to_address(t.operator->());
    }
}
} // namespace v1
} // namespace stdx
