#pragma once

#include <conc/concepts.hpp>

#include <atomic>
#include <memory>
#include <type_traits>

#if __cplusplus >= 202002L
#define CPP20(...) __VA_ARGS__
#else
#define CPP20(...)
#endif

namespace atomic {
namespace detail {
struct standard_policy {
    static_assert(static_cast<int>(std::memory_order_relaxed) ==
                  __ATOMIC_RELAXED);
    static_assert(static_cast<int>(std::memory_order_acquire) ==
                  __ATOMIC_ACQUIRE);
    static_assert(static_cast<int>(std::memory_order_release) ==
                  __ATOMIC_RELEASE);
    static_assert(static_cast<int>(std::memory_order_seq_cst) ==
                  __ATOMIC_SEQ_CST);
    static_assert(static_cast<int>(std::memory_order_acq_rel) ==
                  __ATOMIC_ACQ_REL);
    static_assert(static_cast<int>(std::memory_order_consume) ==
                  __ATOMIC_CONSUME);

    template <typename T>
    static auto load(T const &t,
                     std::memory_order mo = std::memory_order_seq_cst) -> T {
        T ret{};
        __atomic_load(std::addressof(t), std::addressof(ret),
                      static_cast<int>(mo));
        return ret;
    }

    template <typename T>
    static auto
    store(T &t, T &value,
          std::memory_order mo = std::memory_order_seq_cst) -> void {
        __atomic_store(std::addressof(t), std::addressof(value),
                       static_cast<int>(mo));
    }

    template <typename T>
    static auto
    exchange(T &t, T &value,
             std::memory_order mo = std::memory_order_seq_cst) -> T {
        T ret{};
        __atomic_exchange(std::addressof(t), std::addressof(value),
                          std::addressof(ret), static_cast<int>(mo));
        return ret;
    }

    template <typename T>
    static auto
    fetch_add(T &t, T value,
              std::memory_order mo = std::memory_order_seq_cst) -> T {
        return __atomic_fetch_add(std::addressof(t), value,
                                  static_cast<int>(mo));
    }

    template <typename T>
    static auto
    fetch_sub(T &t, T value,
              std::memory_order mo = std::memory_order_seq_cst) -> T {
        return __atomic_fetch_sub(std::addressof(t), value,
                                  static_cast<int>(mo));
    }

    template <typename T>
    static auto
    fetch_and(T &t, T value,
              std::memory_order mo = std::memory_order_seq_cst) -> T {
        return __atomic_fetch_and(std::addressof(t), value,
                                  static_cast<int>(mo));
    }

    template <typename T>
    static auto
    fetch_or(T &t, T value,
             std::memory_order mo = std::memory_order_seq_cst) -> T {
        return __atomic_fetch_or(std::addressof(t), value,
                                 static_cast<int>(mo));
    }

    template <typename T>
    static auto
    fetch_xor(T &t, T value,
              std::memory_order mo = std::memory_order_seq_cst) -> T {
        return __atomic_fetch_xor(std::addressof(t), value,
                                  static_cast<int>(mo));
    }
};
} // namespace detail

template <typename...> inline auto injected_policy = detail::standard_policy{};

template <typename... DummyArgs, typename T>
CPP20(requires(sizeof...(DummyArgs) == 0))
[[nodiscard]] auto load(T const &t,
                        std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(load_store_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.load(t, mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto store(T &t, U value,
           std::memory_order mo = std::memory_order_seq_cst) -> void {
    CPP20(load_store_policy)
    auto &p = injected_policy<DummyArgs...>;
    auto v = static_cast<T>(value);
    p.store(t, v, mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
[[nodiscard]] auto exchange(
    T &t, U value, std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(exchange_policy)
    auto &p = injected_policy<DummyArgs...>;
    auto v = static_cast<T>(value);
    return p.exchange(t, v, mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto fetch_add(T &t, U value,
               std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(add_sub_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.fetch_add(t, static_cast<T>(value), mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto fetch_sub(T &t, U value,
               std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(add_sub_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.fetch_sub(t, static_cast<T>(value), mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto fetch_and(T &t, U value,
               std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(bitwise_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.fetch_and(t, static_cast<T>(value), mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto fetch_or(T &t, U value,
              std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(bitwise_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.fetch_or(t, static_cast<T>(value), mo);
}

template <typename... DummyArgs, typename T, typename U,
          typename = std::enable_if_t<std::is_convertible_v<U, T>>>
CPP20(requires(sizeof...(DummyArgs) == 0))
auto fetch_xor(T &t, U value,
               std::memory_order mo = std::memory_order_seq_cst) -> T {
    CPP20(bitwise_policy)
    auto &p = injected_policy<DummyArgs...>;
    return p.fetch_xor(t, static_cast<T>(value), mo);
}

template <typename T> struct atomic_type {
    using type = T;
};
template <typename T> using atomic_type_t = typename atomic_type<T>::type;

template <typename T>
constexpr inline auto alignment_of = alignof(std::atomic<atomic_type_t<T>>);
} // namespace atomic

#undef CPP20

#ifdef ATOMIC_CFG
#include ATOMIC_CFG
#endif
