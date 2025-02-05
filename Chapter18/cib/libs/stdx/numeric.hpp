#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
#if __cplusplus >= 202002L
#define CONSTEXPR_INVOKE constexpr
#else
#define CONSTEXPR_INVOKE
#endif

template <typename T, typename InputIt, typename ROp, typename TOp,
          typename... InputItN>
CONSTEXPR_INVOKE auto transform_reduce(InputIt first, InputIt last, T init,
                                       ROp rop, TOp top,
                                       InputItN... first_n) -> T {
    while (first != last) {
        init = std::invoke(rop, std::move(init),
                           std::invoke(top, *first, *first_n...));
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return init;
}

template <typename T, typename InputIt, typename Size, typename ROp,
          typename TOp, typename... InputItN>
CONSTEXPR_INVOKE auto transform_reduce_n(InputIt first, Size n, T init, ROp rop,
                                         TOp top, InputItN... first_n) -> T {
    while (n-- != 0) {
        init = std::invoke(rop, std::move(init),
                           std::invoke(top, *first, *first_n...));
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return init;
}
#undef CONSTEXPR_INVOKE

template <typename To, typename From>
constexpr auto saturate_cast(From from) -> To {
    constexpr auto to_min = std::numeric_limits<To>::min();
    constexpr auto to_max = std::numeric_limits<To>::max();

    if constexpr (sizeof(From) > sizeof(To)) {
        auto const clamped = std::clamp<From>(from, to_min, to_max);
        return static_cast<To>(clamped);
    }

    if constexpr (sizeof(From) == sizeof(To)) {
        if constexpr (std::is_unsigned_v<From> and std::is_signed_v<To>) {
            if (from > to_max) {
                return to_max;
            }
        }

        if constexpr (std::is_signed_v<From> and std::is_unsigned_v<To>) {
            if (from < 0) {
                return static_cast<To>(0);
            }
        }
    }

    return static_cast<To>(from);
}

} // namespace v1
} // namespace stdx
