#pragma once

#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {

template <typename E> constexpr auto to_underlying(E e) noexcept {
    if constexpr (std::is_enum_v<E>) {
        return static_cast<std::underlying_type_t<E>>(e);
    } else {
        return e;
    }
}
template <typename E>
using underlying_type_t = decltype(to_underlying(std::declval<E>()));

template <typename T> struct remove_cvref {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;

namespace detail {
template <bool> struct conditional;

template <> struct conditional<true> {
    template <typename T, typename> using choice_t = T;
};
template <> struct conditional<false> {
    template <typename, typename U> using choice_t = U;
};
} // namespace detail

template <bool B, typename T, typename U>
using conditional_t = typename detail::conditional<B>::template choice_t<T, U>;

template <template <typename...> typename P, typename X, typename Y = void>
using type_or_t = conditional_t<P<X>::value, X, Y>;

template <typename...> constexpr bool always_false_v = false;

template <typename T>
constexpr bool is_function_v =
    not std::is_reference_v<T> and not std::is_const_v<std::add_const_t<T>>;

namespace detail {
struct call_base {
    auto operator()() -> void;
};

template <typename, bool> struct callable_test : call_base {};
template <typename F> struct callable_test<F, true> : F, call_base {};

template <typename F, typename = void> constexpr auto is_func_obj = true;
template <typename F>
constexpr auto is_func_obj<
    F,
    std::void_t<decltype(&callable_test<F, std::is_class_v<F>>::operator())>> =
    false;
} // namespace detail
template <typename T>
constexpr bool is_function_object_v = detail::is_func_obj<T>;

template <typename T>
constexpr bool is_callable_v = is_function_v<T> or is_function_object_v<T>;

constexpr auto is_constant_evaluated() noexcept -> bool {
    return __builtin_is_constant_evaluated();
}

template <typename T> struct type_identity {
    using type = T;
};
template <typename T> using type_identity_t = typename type_identity<T>::type;

namespace detail {
template <typename T, template <typename...> typename U>
constexpr bool is_type_specialization_of_v = false;
template <typename... Ts, template <typename...> typename U>
constexpr bool is_type_specialization_of_v<U<Ts...> &, U> = true;
template <typename... Ts, template <typename...> typename U>
constexpr bool is_type_specialization_of_v<U<Ts...> const &, U> = true;

template <typename T, template <auto...> typename U>
constexpr bool is_value_specialization_of_v = false;
template <auto... Vs, template <auto...> typename U>
constexpr bool is_value_specialization_of_v<U<Vs...> &, U> = true;
template <auto... Vs, template <auto...> typename U>
constexpr bool is_value_specialization_of_v<U<Vs...> const &, U> = true;
} // namespace detail

template <typename U, template <typename...> typename T>
constexpr bool is_specialization_of_v =
    detail::is_type_specialization_of_v<U &, T>;
template <typename U, template <typename...> typename T>
constexpr bool is_type_specialization_of_v =
    detail::is_type_specialization_of_v<U &, T>;
template <typename U, template <auto...> typename T>
constexpr bool is_value_specialization_of_v =
    detail::is_value_specialization_of_v<U &, T>;

template <typename U, template <typename...> typename T>
constexpr auto
is_specialization_of() -> std::bool_constant<is_specialization_of_v<U, T>> {
    return {};
}

template <typename U, template <auto...> typename T>
constexpr auto is_specialization_of()
    -> std::bool_constant<is_value_specialization_of_v<U, T>> {
    return {};
}

template <typename E>
constexpr bool is_scoped_enum_v =
    std::is_enum_v<E> and not std::is_convertible_v<E, underlying_type_t<E>>;
template <typename E>
using is_scoped_enum = std::bool_constant<is_scoped_enum_v<E>>;

template <typename...> struct type_list {};
template <auto...> struct value_list {};

namespace detail {
template <typename L> struct for_each_t {
    static_assert(always_false_v<L>,
                  "template_for_each must be called with a type list, "
                  "value_list, or std::integer_sequence");
};

template <template <typename...> typename L, typename... Ts>
struct for_each_t<L<Ts...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        (f.template operator()<Ts>(), ...);
    }
};
template <template <auto...> typename L, auto... Vs>
struct for_each_t<L<Vs...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        (f.template operator()<Vs>(), ...);
    }
};
template <template <typename, auto...> typename L, typename T, T... Vs>
struct for_each_t<L<T, Vs...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        (f.template operator()<Vs>(), ...);
    }
};
} // namespace detail

template <typename L>
constexpr static auto template_for_each = detail::for_each_t<L>{};

namespace detail {
template <typename L> struct apply_sequence_t {
    static_assert(always_false_v<L>,
                  "apply_sequence must be called with a type list, "
                  "value_list, or std::integer_sequence");
};

template <template <typename...> typename L, typename... Ts>
struct apply_sequence_t<L<Ts...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        return f.template operator()<Ts...>();
    }
};
template <template <auto...> typename L, auto... Vs>
struct apply_sequence_t<L<Vs...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        return f.template operator()<Vs...>();
    }
};
template <template <typename, auto...> typename L, typename T, T... Vs>
struct apply_sequence_t<L<T, Vs...>> {
    template <typename F> constexpr auto operator()(F &&f) const {
        return f.template operator()<Vs...>();
    }
};
} // namespace detail

template <typename L>
constexpr static auto apply_sequence = detail::apply_sequence_t<L>{};

template <typename T, typename U>
constexpr bool is_same_unqualified_v =
    std::is_same_v<remove_cvref_t<T>, remove_cvref_t<U>>;

namespace detail {
template <typename T> struct any_t;

template <typename T, typename... Ts> constexpr auto try_construct() -> T {
    if constexpr (std::is_constructible_v<T, Ts...>) {
        return T{Ts{}...};
    } else if constexpr (sizeof...(Ts) < 10) {
        return try_construct<T, Ts..., any_t<T>>();
    } else {
        throw;
    }
}

template <typename T> struct any_t {
    // NOLINTNEXTLINE(modernize-use-constraints)
    template <typename U, std::enable_if_t<not std::is_same_v<T, U>, int> = 0>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr operator U() {
        return try_construct<U>();
    }
};

template <auto> using void_v = void;
template <typename T, typename = void> constexpr auto detect_structural = false;
template <typename T> constexpr auto detect_structural<T &, void> = true;
template <typename T>
constexpr auto detect_structural<T, void_v<try_construct<T>()>> = true;
} // namespace detail

template <typename T>
constexpr bool is_structural_v = detail::detect_structural<T>;

template <typename T, typename = void> constexpr auto is_cx_value_v = false;
template <typename T>
constexpr auto is_cx_value_v<T, std::void_t<typename T::cx_value_t>> = true;

#if __cplusplus >= 202002L
template <typename T>
using shrink_t = decltype([]() -> T (*)() { return nullptr; });

template <typename T> using expand_t = decltype(T{}()());
#endif
} // namespace v1
} // namespace stdx
