#pragma once

#include <stdx/type_traits.hpp>

#include <array>
#include <cstddef>

namespace stdx {
inline namespace v1 {
namespace detail {
template <typename T> struct ct_capacity_fail {
    static_assert(always_false_v<stdx::remove_cvref_t<T>>,
                  "Type does not support compile-time capacity");
};
} // namespace detail

template <typename T>
constexpr auto ct_capacity_v = detail::ct_capacity_fail<T>{};

template <typename T, std::size_t N>
constexpr auto ct_capacity_v<std::array<T, N>> = N;

template <typename T> constexpr auto ct_capacity_v<T const> = ct_capacity_v<T>;

template <typename T> constexpr auto ct_capacity(T &&) -> std::size_t {
    return ct_capacity_v<remove_cvref_t<T>>;
}
} // namespace v1
} // namespace stdx

#if __has_include(<span>)
#include <span>

#if __cpp_lib_span >= 202002L
namespace stdx {
inline namespace v1 {
template <typename T, std::size_t N>
constexpr auto ct_capacity_v<std::span<T, N>> = N;

template <typename T>
constexpr auto ct_capacity_v<std::span<T, std::dynamic_extent>> =
    detail::ct_capacity_fail<std::span<T, std::dynamic_extent>>{};
} // namespace v1
} // namespace stdx

#endif
#endif
