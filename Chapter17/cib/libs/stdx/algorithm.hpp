#pragma once

#include <functional>
#if __cplusplus >= 202002L
#include <stdx/tuple.hpp>
#else
#include <tuple>
#endif

namespace stdx {
inline namespace v1 {
namespace detail {
#if __cplusplus >= 202002L
template <typename... Ts> using result_tuple_t = stdx::tuple<Ts...>;
#define CONSTEXPR_INVOKE constexpr
#else
template <typename... Ts> using result_tuple_t = std::tuple<Ts...>;
#define CONSTEXPR_INVOKE
#endif
} // namespace detail

template <typename O, typename... Is>
using transform_result = detail::result_tuple_t<O, Is...>;

template <typename InputIt, typename OutputIt, typename Operation,
          typename... InputItN>
CONSTEXPR_INVOKE auto transform(InputIt first, InputIt last, OutputIt d_first,
                                Operation op, InputItN... first_n)
    -> transform_result<OutputIt, InputIt, InputItN...> {
    while (first != last) {
        *d_first = std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...),
            ++d_first;
    }
    return {d_first, first, first_n...};
}

template <typename InputIt, typename Size, typename OutputIt,
          typename Operation, typename... InputItN>
CONSTEXPR_INVOKE auto transform_n(InputIt first, Size n, OutputIt d_first,
                                  Operation op, InputItN... first_n)
    -> transform_result<OutputIt, InputIt, InputItN...> {
    while (n-- > 0) {
        *d_first = std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...),
            ++d_first;
    }
    return {d_first, first, first_n...};
}

template <typename InputIt, typename Operation, typename... InputItN>
CONSTEXPR_INVOKE auto for_each(InputIt first, InputIt last, Operation op,
                               InputItN... first_n) -> Operation {
    while (first != last) {
        std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return op;
}

template <typename InputIt, typename Size, typename Operation,
          typename... InputItN>
CONSTEXPR_INVOKE auto for_each_n(InputIt first, Size n, Operation op,
                                 InputItN... first_n) -> Operation {
    while (n-- > 0) {
        std::invoke(op, *first, *first_n...);
        static_cast<void>(++first), (static_cast<void>(++first_n), ...);
    }
    return op;
}

#undef CONSTEXPR_INVOKE
} // namespace v1
} // namespace stdx
