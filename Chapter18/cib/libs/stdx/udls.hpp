#pragma once

#include <stdx/compiler.hpp>
#include <stdx/type_traits.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace stdx {
inline namespace v1 {

namespace detail {
template <char C> constexpr static bool is_digit_sep_v = C == '\'';
template <char C>
constexpr static bool is_decimal_digit_v = C >= '0' and C <= '9';
template <char C>
constexpr static bool is_octal_digit_v = C >= '0' and C <= '7';
template <char C>
constexpr static bool is_binary_digit_v = C >= '0' and C <= '1';

template <char C>
constexpr static char force_lower_case = static_cast<unsigned char>(C) | 32u;
template <char C>
constexpr static bool is_hex_digit_v =
    (C >= '0' and C <= '9') or
    (force_lower_case<C> >= 'a' and force_lower_case<C> <= 'f');

template <char C>
constexpr static auto integral_value_v =
    is_decimal_digit_v<C> ? C - '0' : force_lower_case<C> - 'a' + 10;

template <auto Base, char C, typename Sum>
CONSTEVAL auto maybe_add_digit(Sum s) {
    if constexpr (not is_digit_sep_v<C>) {
        s *= Base;
        s += integral_value_v<C>;
    }
    return s;
}

template <auto Base, char... Cs> struct raw_parser {
    template <typename T> CONSTEVAL static auto parse() {
        using U = decltype(stdx::to_underlying(std::declval<T>()));
        auto x = U{};
        ((x = maybe_add_digit<Base, Cs>(x)), ...);
        return T{x};
    }
};

template <char... Cs> struct parser : raw_parser<10, Cs...> {
    static_assert((... and (is_decimal_digit_v<Cs> or is_digit_sep_v<Cs>)));
};

template <char... Cs> struct parser<'0', Cs...> : raw_parser<8, Cs...> {
    static_assert((... and (is_octal_digit_v<Cs> or is_digit_sep_v<Cs>)));
};

template <char... Cs> struct parser<'0', 'x', Cs...> : raw_parser<16, Cs...> {
    static_assert((... and (is_hex_digit_v<Cs> or is_digit_sep_v<Cs>)));
};
template <char... Cs>
struct parser<'0', 'X', Cs...> : parser<'0', 'x', Cs...> {};

template <char... Cs> struct parser<'0', 'b', Cs...> : raw_parser<2, Cs...> {
    static_assert((... and (is_binary_digit_v<Cs> or is_digit_sep_v<Cs>)));
};
template <char... Cs>
struct parser<'0', 'B', Cs...> : parser<'0', 'b', Cs...> {};
} // namespace detail

template <typename T, char... Chars> CONSTEVAL auto parse_literal() -> T {
    using parser_t = detail::parser<Chars...>;
    return parser_t::template parse<T>();
}

template <auto I> using constant = std::integral_constant<decltype(I), I>;
template <auto I> constexpr static constant<I> _c{};

inline namespace literals {
template <char... Chars> CONSTEVAL auto operator""_c() {
    return _c<parse_literal<std::uint32_t, Chars...>()>;
}
} // namespace literals

enum struct lsb_t : std::uint32_t {};
enum struct msb_t : std::uint32_t {};
enum struct length_t : std::uint32_t {};

inline namespace literals {
// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto operator""_lsb(unsigned long long int n) -> lsb_t {
    return static_cast<lsb_t>(n);
}
CONSTEVAL auto operator""_msb(unsigned long long int n) -> msb_t {
    return static_cast<msb_t>(n);
}
CONSTEVAL auto operator""_len(unsigned long long int n) -> length_t {
    return static_cast<length_t>(n);
}
// NOLINTEND(google-runtime-int)
} // namespace literals

inline namespace literals {
CONSTEVAL auto operator""_b(char const *, std::size_t) -> bool { return true; }
CONSTEVAL auto operator""_true(char const *, std::size_t) -> bool {
    return true;
}
CONSTEVAL auto operator""_false(char const *, std::size_t) -> bool {
    return false;
}

// NOLINTBEGIN(google-runtime-int)
CONSTEVAL auto
operator""_k(unsigned long long int n) -> unsigned long long int {
    return n * 1'000u;
}
CONSTEVAL auto
operator""_M(unsigned long long int n) -> unsigned long long int {
    return n * 1'000'000u;
}
CONSTEVAL auto
operator""_G(unsigned long long int n) -> unsigned long long int {
    return n * 1'000'000'000u;
}

CONSTEVAL auto
operator""_ki(unsigned long long int n) -> unsigned long long int {
    return n * 1'024u;
}
CONSTEVAL auto
operator""_Mi(unsigned long long int n) -> unsigned long long int {
    return n * 1'024ull * 1'024ull;
}
CONSTEVAL auto
operator""_Gi(unsigned long long int n) -> unsigned long long int {
    return n * 1'024ull * 1'024ull * 1'024ull;
}

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define STDX_SMALL_INT_LITERAL_DEF(x)                                          \
    CONSTEVAL auto operator""_##x(char const *, std::size_t)                   \
        ->std::integral_constant<std::size_t, x##u> {                          \
        return {};                                                             \
    }

STDX_SMALL_INT_LITERAL_DEF(0)
STDX_SMALL_INT_LITERAL_DEF(1)
STDX_SMALL_INT_LITERAL_DEF(2)
STDX_SMALL_INT_LITERAL_DEF(3)
STDX_SMALL_INT_LITERAL_DEF(4)
STDX_SMALL_INT_LITERAL_DEF(5)
STDX_SMALL_INT_LITERAL_DEF(6)
STDX_SMALL_INT_LITERAL_DEF(7)
STDX_SMALL_INT_LITERAL_DEF(8)
STDX_SMALL_INT_LITERAL_DEF(9)

#undef STDX_SMALL_INT_LITERAL_DEF

// NOLINTEND(cppcoreguidelines-macro-usage)

// NOLINTEND(google-runtime-int)
} // namespace literals

} // namespace v1
} // namespace stdx
