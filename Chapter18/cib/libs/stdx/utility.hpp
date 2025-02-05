#pragma once

#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/type_traits.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {

template <typename... Fs> struct overload : Fs... {
    using Fs::operator()...;
};

#if __cpp_deduction_guides < 201907L
template <typename... Fs> overload(Fs...) -> overload<Fs...>;
#endif

[[noreturn]] inline auto unreachable() -> void { __builtin_unreachable(); }

namespace detail {
template <auto V> struct value_t {
    constexpr static inline auto value = V;
};
} // namespace detail

template <typename K, typename V> struct type_pair {};
template <typename K, typename V> using tt_pair = type_pair<K, V>;
template <auto K, typename V> using vt_pair = tt_pair<detail::value_t<K>, V>;
template <typename K, auto V> using tv_pair = tt_pair<K, detail::value_t<V>>;
template <auto K, auto V>
using vv_pair = tt_pair<detail::value_t<K>, detail::value_t<V>>;

template <typename... Ts> struct type_map : Ts... {};

namespace detail {
template <typename K, typename Default>
constexpr static auto lookup(...) -> Default;
template <typename K, typename Default, typename V>
constexpr static auto lookup(type_pair<K, V>) -> V;
} // namespace detail

template <typename M, typename K, typename Default = void>
using type_lookup_t = decltype(detail::lookup<K, Default>(std::declval<M>()));

template <typename M, auto K, typename Default = void>
using value_lookup_t =
    decltype(detail::lookup<detail::value_t<K>, Default>(std::declval<M>()));

namespace detail {
template <typename T>
using is_not_void = std::bool_constant<not std::is_void_v<T>>;
}

template <typename M, typename K, auto Default = 0>
constexpr static auto type_lookup_v =
    type_or_t<detail::is_not_void,
              decltype(detail::lookup<K, void>(std::declval<M>())),
              detail::value_t<Default>>::value;

template <typename M, auto K, auto Default = 0>
constexpr static auto value_lookup_v =
    type_or_t<detail::is_not_void,
              decltype(detail::lookup<detail::value_t<K>, void>(
                  std::declval<M>())),
              detail::value_t<Default>>::value;

#if __cpp_lib_forward_like < 202207L
template <typename T, typename U>
[[nodiscard]] constexpr auto forward_like(U &&u) noexcept -> decltype(auto) {
    constexpr auto t_is_const = std::is_const_v<std::remove_reference_t<T>>;

    if constexpr (std::is_lvalue_reference_v<T &&>) {
        if constexpr (t_is_const) {
            return std::as_const(u);
        } else {
            return (u);
        }
    } else {
        if constexpr (t_is_const) {
            return std::move(std::as_const(u));
        } else {
            return static_cast<U &&>(u);
        }
    }
}
#else
using std::forward_like;
#endif
template <typename T, typename U>
using forward_like_t = decltype(forward_like<T>(std::declval<U>()));

template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
[[nodiscard]] auto as_unsigned(T t) {
    static_assert(not std::is_same_v<T, bool>,
                  "as_unsigned is not applicable to bool");
    return static_cast<std::make_unsigned_t<T>>(t);
}

template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
[[nodiscard]] auto as_signed(T t) {
    static_assert(not std::is_same_v<T, bool>,
                  "as_signed is not applicable to bool");
    return static_cast<std::make_signed_t<T>>(t);
}

namespace detail {
template <typename T, typename U>
[[nodiscard]] constexpr auto size_conversion(std::size_t sz) -> std::size_t {
    if constexpr (sizeof(T) == sizeof(U)) {
        return sz;
    } else if constexpr (sizeof(T) > sizeof(U)) {
        return sz * (sizeof(T) / sizeof(U));
    } else {
        return (sz * sizeof(T) + sizeof(U) - 1) / sizeof(U);
    }
}
} // namespace detail

template <typename T> struct sized {
    template <typename U = std::uint8_t>
    [[nodiscard]] constexpr auto in() -> std::size_t {
        return detail::size_conversion<T, U>(sz);
    }

    std::size_t sz;
};

using sized8 = sized<std::uint8_t>;
using sized16 = sized<std::uint16_t>;
using sized32 = sized<std::uint32_t>;
using sized64 = sized<std::uint64_t>;

namespace cxv_detail {
struct from_any {
    // NOLINTNEXTLINE(google-explicit-constructor)
    template <typename... Ts> constexpr from_any(Ts const &...) {}
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator int() const { return 0; }
};

struct type_val {
    template <typename T, typename U,
              typename = std::enable_if_t<same_as_unqualified<type_val, U>>>
    friend constexpr auto operator+(T &&t, U &&) -> T {
        return t;
    }
    friend constexpr auto operator+(type_val const &f) -> type_val { return f; }
    // NOLINTNEXTLINE(google-explicit-constructor)
    template <typename T> constexpr operator T() const {
        extern auto cxv_type_val_get_t(T *) -> T;
        return cxv_type_val_get_t(nullptr);
    }
};

template <int> constexpr auto is_type() -> std::false_type;
template <typename> constexpr auto is_type() -> std::true_type;

template <typename> struct typer;
template <typename T> struct typer<from_any(T)> {
    using type = T;
};

template <int> constexpr auto type_of() -> void;
template <typename T> constexpr auto type_of() -> typename typer<T>::type;

class cx_base {
    struct unusable {};

  public:
    using cx_value_t [[maybe_unused]] = void;
    constexpr auto operator()(unusable) const {}
};
} // namespace cxv_detail

template <typename T>
constexpr auto is_aligned_with = [](auto v) -> bool {
    static_assert(std::is_integral_v<decltype(v)> or
                      std::is_pointer_v<decltype(v)>,
                  "is_aligned_with should only be used with an integral or "
                  "pointer argument!");
    constexpr auto mask = alignof(T) - 1u;
    if constexpr (std::is_pointer_v<decltype(v)>) {
        return (__builtin_bit_cast(std::uintptr_t, v) & mask) == 0;
    } else {
        return (static_cast<std::uintptr_t>(v) & mask) == 0;
    }
};

#if __cplusplus >= 202002L

namespace detail {
template <typename T> struct ct_helper {
    // NOLINTNEXTLINE(google-explicit-constructor)
    CONSTEVAL ct_helper(T t) : value(t) {}
    T value;
};
template <typename T> ct_helper(T) -> ct_helper<T>;
} // namespace detail

template <detail::ct_helper Value> CONSTEVAL auto ct() {
    return std::integral_constant<decltype(Value.value), Value.value>{};
}
template <typename T> CONSTEVAL auto ct() { return type_identity<T>{}; }

#endif
} // namespace v1
} // namespace stdx

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifndef FWD
#define FWD(x) std::forward<decltype(x)>(x)
#endif

#ifndef CX_VALUE
#define CX_VALUE(...)                                                          \
    []() constexpr {                                                           \
        STDX_PRAGMA(diagnostic push)                                           \
        STDX_PRAGMA(diagnostic ignored "-Wold-style-cast")                     \
        STDX_PRAGMA(diagnostic ignored "-Wunused-value")                       \
        if constexpr (decltype(stdx::cxv_detail::is_type<                      \
                               stdx::cxv_detail::from_any(                     \
                                   __VA_ARGS__)>())::value) {                  \
            return stdx::overload{stdx::cxv_detail::cx_base{}, [] {            \
                                      return stdx::type_identity<              \
                                          decltype(stdx::cxv_detail::type_of<  \
                                                   stdx::cxv_detail::from_any( \
                                                       __VA_ARGS__)>())>{};    \
                                  }};                                          \
        } else {                                                               \
            return stdx::overload{stdx::cxv_detail::cx_base{}, [] {            \
                                      return (__VA_ARGS__) +                   \
                                             stdx::cxv_detail::type_val{};     \
                                  }};                                          \
        }                                                                      \
        STDX_PRAGMA(diagnostic pop)                                            \
    }()
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)
// NOLINTEND(modernize-use-constraints)
