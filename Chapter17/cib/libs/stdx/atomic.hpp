#pragma once

#include <conc/atomic.hpp>

#include <atomic>
#include <type_traits>

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if __cplusplus >= 202002L
#define CPP20(...) __VA_ARGS__
#else
#define CPP20(...)
#endif

namespace stdx {
inline namespace v1 {
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
template <typename T> class atomic {
    static_assert(std::is_trivially_copyable_v<T> and
                      std::is_copy_constructible_v<T> and
                      std::is_move_constructible_v<T> and
                      std::is_copy_assignable_v<T> and
                      std::is_move_assignable_v<T>,
                  "Atomic values must be trivially copyable, copy "
                  "constructible and copy assignable");

    using elem_t = ::atomic::atomic_type_t<T>;
    constexpr static auto alignment = ::atomic::alignment_of<T>;

    static_assert(std::is_convertible_v<elem_t, T>,
                  "::atomic::atomic_type_t specialization result must be "
                  "convertible to T");
    static_assert(std::is_convertible_v<T, elem_t>,
                  "::atomic::atomic_type_t specialization result must be "
                  "convertible from T");

    alignas(alignment) elem_t value;

  public:
    using value_type = T;

    constexpr atomic() CPP20(requires std::is_default_constructible_v<elem_t>)
        : value{} {}
    constexpr explicit atomic(T t) : value{static_cast<elem_t>(t)} {}
    atomic(atomic const &) = delete;
    auto operator=(atomic const &) -> atomic & = delete;

    [[nodiscard]] auto
    load(std::memory_order mo = std::memory_order_seq_cst) const -> T {
        return static_cast<T>(::atomic::load(value, mo));
    }

    void store(T t, std::memory_order mo = std::memory_order_seq_cst) {
        ::atomic::store(value, static_cast<elem_t>(t), mo);
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[nodiscard]] operator T() const { return load(); }
    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    auto operator=(T t) -> T {
        store(t);
        return t;
    }

    [[nodiscard]] auto
    exchange(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        return ::atomic::exchange(value, static_cast<elem_t>(t), mo);
    }

    auto fetch_add(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        CPP20(static_assert(
            requires { t + t; }, "T must support operator+(x, y)"));
        return ::atomic::fetch_add(value, static_cast<elem_t>(t), mo);
    }
    auto fetch_sub(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        CPP20(static_assert(
            requires { t - t; }, "T must support operator-(x, y)"));
        return ::atomic::fetch_sub(value, static_cast<elem_t>(t), mo);
    }

    auto operator+=(T t) -> T { return fetch_add(t) + t; }
    auto operator-=(T t) -> T { return fetch_sub(t) - t; }

    auto operator++() -> T {
        CPP20(static_assert(
            requires(T t) { ++t; }, "T must support operator++()"));
        return ::atomic::fetch_add(value, 1) + 1;
    }
    [[nodiscard]] auto operator++(int) -> T {
        CPP20(static_assert(
            requires(T t) { t++; }, "T must support operator++(int)"));
        return ::atomic::fetch_add(value, 1);
    }
    auto operator--() -> T {
        CPP20(static_assert(
            requires(T t) { --t; }, "T must support operator--()"));
        return ::atomic::fetch_sub(value, 1) - 1;
    }
    [[nodiscard]] auto operator--(int) -> T {
        CPP20(static_assert(
            requires(T t) { t--; }, "T must support operator--(int)"));
        return ::atomic::fetch_sub(value, 1);
    }

    auto fetch_and(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        CPP20(static_assert(
            requires { t & t; }, "T must support operator&(x, y)"));
        return ::atomic::fetch_and(value, static_cast<elem_t>(t), mo);
    }
    auto fetch_or(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        CPP20(static_assert(
            requires { t | t; }, "T must support operator|(x, y)"));
        return ::atomic::fetch_or(value, static_cast<elem_t>(t), mo);
    }
    auto fetch_xor(T t, std::memory_order mo = std::memory_order_seq_cst) -> T {
        CPP20(static_assert(
            requires { t ^ t; }, "T must support operator^(x, y)"));
        return ::atomic::fetch_xor(value, static_cast<elem_t>(t), mo);
    }

    auto operator&=(T t) -> T { return fetch_and(t) & t; }
    auto operator|=(T t) -> T { return fetch_or(t) | t; }
    auto operator^=(T t) -> T { return fetch_xor(t) ^ t; }
};
} // namespace v1
} // namespace stdx

// NOLINTEND(cppcoreguidelines-macro-usage)
