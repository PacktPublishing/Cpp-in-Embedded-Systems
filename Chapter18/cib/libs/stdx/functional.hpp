#pragma once

#include <stdx/type_traits.hpp>

#include <functional>
#include <type_traits>

#if __cpp_lib_bind_front < 202306L or __cpp_lib_bind_back < 202306L
#if __cplusplus >= 202002L
#include <stdx/tuple.hpp>
#else
#include <tuple>
#endif
#endif

namespace stdx {
inline namespace v1 {

template <typename F> struct with_result_of : F {
    using R = std::invoke_result_t<F>;
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator R() const
        noexcept(noexcept(static_cast<F const &>(*this)())) {
        return static_cast<F const &>(*this)();
    }
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator R() noexcept(noexcept(static_cast<F &>(*this)())) {
        return static_cast<F &>(*this)();
    }
};

#if __cpp_deduction_guides < 201907L
template <typename F> with_result_of(F) -> with_result_of<F>;
#endif

namespace detail {
#if __cpp_lib_bind_front < 202306L or __cpp_lib_bind_back < 202306L
#if __cplusplus >= 202002L
template <typename... Ts> using bind_tuple_t = stdx::tuple<Ts...>;
using stdx::get;
#else
template <typename... Ts> using bind_tuple_t = std::tuple<Ts...>;
using std::get;
#endif
#endif
} // namespace detail

#if __cpp_lib_bind_front >= 201907L
using std::bind_front;
#else
namespace detail {
template <typename...> struct bind_front_t;

template <typename F, std::size_t... Is, typename... BoundArgs>
struct bind_front_t<F, std::index_sequence<Is...>, BoundArgs...> {
    F f{};
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return f(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return f(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return std::move(f)(get<Is>(std::move(t))...,
                            std::forward<Args>(args)...);
    }
};
} // namespace detail

template <typename F, typename... Args>
constexpr auto bind_front(F &&f, Args &&...args) {
    return detail::bind_front_t<stdx::remove_cvref_t<F>,
                                std::make_index_sequence<sizeof...(Args)>,
                                std::decay_t<Args>...>{
        std::forward<F>(f), {std::forward<Args>(args)...}};
}
#endif

#if __cpp_lib_bind_front < 202306L
namespace detail {
template <auto, typename...> struct bind_front_value_t;

template <auto F, std::size_t... Is, typename... BoundArgs>
struct bind_front_value_t<F, std::index_sequence<Is...>, BoundArgs...> {
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return F(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return F(get<Is>(t)..., std::forward<Args>(args)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return F(get<Is>(std::move(t))..., std::forward<Args>(args)...);
    }
};
} // namespace detail

template <auto F, typename... Args> constexpr auto bind_front(Args &&...args) {
    return detail::bind_front_value_t<
        F, std::make_index_sequence<sizeof...(Args)>, std::decay_t<Args>...>{
        {std::forward<Args>(args)...}};
}

#endif

#if __cpp_lib_bind_back >= 202202L
using std::bind_back;
#else
namespace detail {
template <typename...> struct bind_back_t;

template <typename F, std::size_t... Is, typename... BoundArgs>
struct bind_back_t<F, std::index_sequence<Is...>, BoundArgs...> {
    F f{};
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return f(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return f(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return std::move(f)(std::forward<Args>(args)...,
                            get<Is>(std::move(t))...);
    }
};
} // namespace detail

template <typename F, typename... Args>
constexpr auto bind_back(F &&f, Args &&...args) {
    return detail::bind_back_t<stdx::remove_cvref_t<F>,
                               std::make_index_sequence<sizeof...(Args)>,
                               std::decay_t<Args>...>{
        std::forward<F>(f), {std::forward<Args>(args)...}};
}
#endif

#if __cpp_lib_bind_back < 202306L
namespace detail {
template <auto, typename...> struct bind_back_value_t;

template <auto F, std::size_t... Is, typename... BoundArgs>
struct bind_back_value_t<F, std::index_sequence<Is...>, BoundArgs...> {
    bind_tuple_t<BoundArgs...> t{};

    template <typename... Args>
    constexpr auto operator()(Args &&...args) const & {
        return F(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) & {
        return F(std::forward<Args>(args)..., get<Is>(t)...);
    }
    template <typename... Args> constexpr auto operator()(Args &&...args) && {
        return F(std::forward<Args>(args)..., get<Is>(std::move(t))...);
    }
};
} // namespace detail

template <auto F, typename... Args> constexpr auto bind_back(Args &&...args) {
    return detail::bind_back_value_t<
        F, std::make_index_sequence<sizeof...(Args)>, std::decay_t<Args>...>{
        {std::forward<Args>(args)...}};
}

#endif
} // namespace v1
} // namespace stdx
