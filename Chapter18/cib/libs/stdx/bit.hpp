#pragma once

#include <stdx/concepts.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/utility.hpp>

#include <array>
#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>

#if __has_include(<bit>)
#include <bit>
#endif

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {

// endian

#if __cpp_lib_endian < 201907L
// NOLINTNEXTLINE(performance-enum-size)
enum struct endian : underlying_type_t<decltype(__BYTE_ORDER__)> {
    little = __ORDER_LITTLE_ENDIAN__,
    big = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
};
#else
using endian = std::endian;
#endif

// bit_cast

#if __cpp_lib_bit_cast < 201806L
template <typename To, typename From>
[[nodiscard]] constexpr auto bit_cast(From &&from) noexcept -> To {
    return __builtin_bit_cast(To, from);
}
#else
using std::bit_cast;
#endif

// byteswap

#if __cpp_lib_byteswap < 202110L
template <typename T>
[[nodiscard]] constexpr auto
byteswap(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
        return __builtin_bswap16(x);
    } else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
        return __builtin_bswap32(x);
    } else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
        return __builtin_bswap64(x);
    } else {
        return x;
    }
}
#else
using std::byteswap;
#endif

// bit ops

#if __cpp_lib_bitops < 201907L
template <typename T>
[[nodiscard]] constexpr auto
countl_zero(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    if (x == 0) {
        return std::numeric_limits<T>::digits;
    }
    if constexpr (sizeof(T) == sizeof(unsigned int)) {
        return __builtin_clz(x);
    } else if constexpr (sizeof(T) ==
                         sizeof(unsigned long)) { // NOLINT(google-runtime-int)
        return __builtin_clzl(x);
    } else if constexpr (
        sizeof(T) == sizeof(unsigned long long)) { // NOLINT(google-runtime-int)
        return __builtin_clzll(x);
    } else {
        return __builtin_clzll(x) + std::numeric_limits<T>::digits -
               std::numeric_limits<
                   unsigned long long>::digits; // NOLINT(google-runtime-int)
    }
}

template <typename T>
[[nodiscard]] constexpr auto
countr_zero(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    if (x == 0) {
        return std::numeric_limits<T>::digits;
    }
    if constexpr (sizeof(T) == sizeof(unsigned int)) {
        return __builtin_ctz(x);
    } else if constexpr (sizeof(T) ==
                         sizeof(unsigned long)) { // NOLINT(google-runtime-int)
        return __builtin_ctzl(x);
    } else {
        return __builtin_ctzll(x);
    }
}

template <typename T>
[[nodiscard]] constexpr auto
countl_one(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    return countl_zero(T(~x));
}

template <typename T>
[[nodiscard]] constexpr auto
countr_one(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    return countr_zero(T(~x));
}

template <typename T>
[[nodiscard]] constexpr auto
popcount(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    if constexpr (sizeof(T) <= sizeof(unsigned int)) {
        return __builtin_popcount(x);
    } else if constexpr (sizeof(T) <=
                         sizeof(unsigned long)) { // NOLINT(google-runtime-int)
        return __builtin_popcountl(x);
    } else {
        return __builtin_popcountll(x);
    }
}

namespace detail {
template <typename T>
[[nodiscard]] constexpr auto
rotl(T x, T s) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
#ifdef __clang__
    if constexpr (sizeof(T) == sizeof(std::uint8_t)) {
        return __builtin_rotateleft8(x, s);
    } else if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
        return __builtin_rotateleft16(x, s);
    } else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
        return __builtin_rotateleft32(x, s);
    } else {
        return __builtin_rotateleft64(x, s);
    }
#else
    return static_cast<T>((x << s) |
                          (x >> (std::numeric_limits<T>::digits - s)));
#endif
}
template <typename T>
[[nodiscard]] constexpr auto
rotr(T x, T s) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
#ifdef __clang__
    if constexpr (sizeof(T) == sizeof(std::uint8_t)) {
        return __builtin_rotateright8(x, s);
    } else if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
        return __builtin_rotateright16(x, s);
    } else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
        return __builtin_rotateright32(x, s);
    } else {
        return __builtin_rotateright64(x, s);
    }
#else
    return static_cast<T>((x >> s) |
                          (x << (std::numeric_limits<T>::digits - s)));
#endif
}
} // namespace detail

template <typename T>
[[nodiscard]] constexpr auto
rotl(T x, int s) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if (s == 0) {
        return x;
    }
    if (s < 0) {
        return detail::rotr(x, T(-s));
    }
    return detail::rotl(x, T(s));
}

template <typename T>
[[nodiscard]] constexpr auto
rotr(T x, int s) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if (s == 0) {
        return x;
    }
    if (s < 0) {
        return detail::rotl(x, T(-s));
    }
    return detail::rotr(x, T(s));
}
#else
using std::countl_one;
using std::countl_zero;
using std::countr_one;
using std::countr_zero;
using std::popcount;
using std::rotl;
using std::rotr;
#endif

// pow2

#if __cpp_lib_int_pow2 < 202002L
template <typename T>
[[nodiscard]] constexpr auto
has_single_bit(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, bool> {
    return x and not(x & (x - 1));
}

template <typename T>
[[nodiscard]] constexpr auto
bit_width(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, int> {
    return std::numeric_limits<T>::digits - countl_zero(x);
}

template <typename T>
[[nodiscard]] constexpr auto
bit_ceil(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if (x <= 1U) {
        return 1U;
    }
    return T(1U << bit_width(x));
}

template <typename T>
[[nodiscard]] constexpr auto
bit_floor(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if (x == 0) {
        return x;
    }
    return T(1U << (bit_width(x) - 1));
}
#else
using std::bit_ceil;
using std::bit_floor;
using std::bit_width;
using std::has_single_bit;
#endif

template <typename T>
[[nodiscard]] constexpr auto
to_le(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if constexpr (stdx::endian::native == stdx::endian::big) {
        return byteswap(x);
    } else {
        return x;
    }
}

template <typename T>
[[nodiscard]] constexpr auto
to_be(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if constexpr (stdx::endian::native == stdx::endian::little) {
        return byteswap(x);
    } else {
        return x;
    }
}

template <typename T>
[[nodiscard]] constexpr auto
from_le(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if constexpr (stdx::endian::native == stdx::endian::big) {
        return byteswap(x);
    } else {
        return x;
    }
}

template <typename T>
[[nodiscard]] constexpr auto
from_be(T x) noexcept -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    if constexpr (stdx::endian::native == stdx::endian::little) {
        return byteswap(x);
    } else {
        return x;
    }
}

template <typename To>
constexpr auto bit_pack = [](auto... args) {
    static_assert(stdx::always_false_v<To, decltype(args)...>,
                  "bit_pack is undefined for those types");
};

template <>
constexpr inline auto bit_pack<std::uint16_t> =
    [](std::uint8_t hi, std::uint8_t lo) -> std::uint16_t {
    return static_cast<std::uint16_t>((static_cast<std::uint32_t>(hi) << 8u) |
                                      lo);
};

template <>
constexpr inline auto bit_pack<std::uint32_t> =
    stdx::overload{[](std::uint16_t hi, std::uint16_t lo) -> std::uint32_t {
                       return (static_cast<std::uint32_t>(hi) << 16u) | lo;
                   },
                   [](std::uint8_t b0, std::uint8_t b1, std::uint8_t b2,
                      std::uint8_t b3) -> std::uint32_t {
                       return (static_cast<std::uint32_t>(b0) << 24u) |
                              (static_cast<std::uint32_t>(b1) << 16u) |
                              (static_cast<std::uint32_t>(b2) << 8u) | b3;
                   }};

template <>
constexpr inline auto bit_pack<std::uint64_t> =
    stdx::overload{[](std::uint32_t hi, std::uint32_t lo) -> std::uint64_t {
                       return (static_cast<std::uint64_t>(hi) << 32u) | lo;
                   },
                   [](std::uint16_t w0, std::uint16_t w1, std::uint16_t w2,
                      std::uint16_t w3) -> std::uint64_t {
                       return (static_cast<std::uint64_t>(w0) << 48u) |
                              (static_cast<std::uint64_t>(w1) << 32u) |
                              (static_cast<std::uint64_t>(w2) << 16u) | w3;
                   },
                   [](std::uint8_t b0, std::uint8_t b1, std::uint8_t b2,
                      std::uint8_t b3, std::uint8_t b4, std::uint8_t b5,
                      std::uint8_t b6, std::uint8_t b7) -> std::uint64_t {
                       return (static_cast<std::uint64_t>(b0) << 56u) |
                              (static_cast<std::uint64_t>(b1) << 48u) |
                              (static_cast<std::uint64_t>(b2) << 40u) |
                              (static_cast<std::uint64_t>(b3) << 32u) |
                              (static_cast<std::uint64_t>(b4) << 24u) |
                              (static_cast<std::uint64_t>(b5) << 16u) |
                              (static_cast<std::uint64_t>(b6) << 8u) | b7;
                   }};

template <typename To, typename From> constexpr auto bit_unpack(From arg) {
    static_assert(unsigned_integral<To> and unsigned_integral<From>,
                  "bit_unpack is undefined for those types");

    constexpr auto sz = sized<From>{1}.template in<To>();
    auto r = bit_cast<std::array<To, sz>>(to_be(arg));
    for (auto &elem : r) {
        elem = from_be(elem);
    }
    return r;
}

namespace detail {
template <typename T, std::size_t Bit>
constexpr auto
mask_bits() -> std::enable_if_t<Bit <= std::numeric_limits<T>::digits, T> {
    if constexpr (Bit == std::numeric_limits<T>::digits) {
        return std::numeric_limits<T>::max();
    } else {
        return static_cast<T>(T{1} << Bit) - T{1};
    }
}

template <typename T> constexpr auto mask_bits(std::size_t Bit) -> T {
    if (Bit == std::numeric_limits<T>::digits) {
        return std::numeric_limits<T>::max();
    }
    return static_cast<T>(T{1} << Bit) - T{1};
}
} // namespace detail

template <typename T, std::size_t Msb = std::numeric_limits<T>::digits - 1,
          std::size_t Lsb = 0>
[[nodiscard]] constexpr auto bit_mask() noexcept
    -> std::enable_if_t<std::is_unsigned_v<T> and Msb >= Lsb, T> {
    static_assert(Msb < std::numeric_limits<T>::digits);
    return detail::mask_bits<T, Msb + 1>() - detail::mask_bits<T, Lsb>();
}

template <typename T>
[[nodiscard]] constexpr auto bit_mask(std::size_t Msb,
                                      std::size_t Lsb = 0) noexcept
    -> std::enable_if_t<std::is_unsigned_v<T>, T> {
    return detail::mask_bits<T>(Msb + 1) - detail::mask_bits<T>(Lsb);
}

template <typename T> constexpr auto bit_size() -> std::size_t {
    return sizeof(T) * CHAR_BIT;
}

template <std::size_t N> CONSTEVAL auto smallest_uint() {
    if constexpr (N <= std::numeric_limits<std::uint8_t>::digits) {
        return std::uint8_t{};
    } else if constexpr (N <= std::numeric_limits<std::uint16_t>::digits) {
        return std::uint16_t{};
    } else if constexpr (N <= std::numeric_limits<std::uint32_t>::digits) {
        return std::uint32_t{};
    } else {
        return std::uint64_t{};
    }
}

template <std::size_t N> using smallest_uint_t = decltype(smallest_uint<N>());
} // namespace v1
} // namespace stdx

// NOLINTEND(modernize-use-constraints)
