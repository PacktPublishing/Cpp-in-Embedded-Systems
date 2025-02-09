#pragma once

#ifdef SIMULATE_FREESTANDING
#define HAS_MUTEX 0
#else
#define HAS_MUTEX __has_include(<mutex>)
#endif

#include <conc/concepts.hpp>

#if HAS_MUTEX
#include <mutex>
#endif

#include <concepts>
#include <utility>

namespace conc {
namespace detail {
template <typename...> constexpr auto always_false_v = false;

#if HAS_MUTEX
template <typename Mutex = std::mutex> class standard_policy {
    template <typename> static inline Mutex m{};

  public:
    template <typename Uniq = void, std::invocable F, std::predicate... Pred>
        requires(sizeof...(Pred) < 2)
    static auto call_in_critical_section(F &&f, Pred &&...pred)
        -> decltype(std::forward<F>(f)()) {
        while (true) {
            [[maybe_unused]] std::lock_guard l{m<Uniq>};
            if ((... and pred())) {
                return std::forward<F>(f)();
            }
        }
    }
};
#else
template <typename = void> struct standard_policy {
    template <typename = void, std::invocable F, std::predicate... Pred>
        requires(sizeof...(Pred) < 2)
    static auto call_in_critical_section(F &&f, Pred &&...)
        -> decltype(std::forward<F>(f)()) {
        static_assert(always_false_v<F, Pred...>,
                      "No standard concurrency policy defined: inject a policy "
                      "by specializing conc::injected_policy<>");
        return std::forward<F>(f)();
    }
};
#endif
} // namespace detail

template <typename...> inline auto injected_policy = detail::standard_policy{};

template <typename Uniq = decltype([] {}), typename... DummyArgs,
          std::invocable F, std::predicate... Pred>
    requires(sizeof...(DummyArgs) == 0 and sizeof...(Pred) < 2)
auto call_in_critical_section(F &&f, Pred &&...pred)
    -> decltype(std::forward<F>(f)()) {
    policy auto &p = injected_policy<DummyArgs...>;
    return p.template call_in_critical_section<Uniq>(
        std::forward<F>(f), std::forward<Pred>(pred)...);
}
} // namespace conc

#undef HAS_MUTEX
