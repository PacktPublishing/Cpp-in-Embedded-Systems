#pragma once

#include <iterator>

namespace stdx {
inline namespace v1 {

#if __cplusplus < 202002L

// NOLINTBEGIN(bugprone-macro-parentheses, cppcoreguidelines-macro-usage)
#define DETECTOR(name, expr)                                                   \
    namespace detail::detect {                                                 \
    template <typename T, typename = void> constexpr auto name = false;        \
    template <typename T>                                                      \
    constexpr auto name<T, std::void_t<decltype(expr)>> = true;                \
    }
// NOLINTEND(bugprone-macro-parentheses, cppcoreguidelines-macro-usage)

DETECTOR(range_begin, (std::begin(std::declval<T &>())))
DETECTOR(range_end, (std::end(std::declval<T &>())))

template <typename T>
constexpr auto range =
    detail::detect::range_begin<T> and detail::detect::range_end<T>;

#undef DETECTOR

#else

template <typename T>
concept range = requires(T &t) {
    std::begin(t);
    std::end(t);
};

#endif

} // namespace v1
} // namespace stdx
