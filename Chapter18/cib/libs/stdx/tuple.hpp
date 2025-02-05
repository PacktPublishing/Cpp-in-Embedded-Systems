#pragma once

#if __cplusplus >= 202002L

#include <stdx/udls.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {

template <std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;
template <std::size_t I> constexpr static index_constant<I> index{};

inline namespace literals {
template <char... Chars> CONSTEVAL auto operator""_idx() {
    return index<parse_literal<std::size_t, Chars...>()>;
}
} // namespace literals

template <typename> struct tag_constant;
template <typename T> constexpr static tag_constant<T> *tag{};

namespace error {
template <typename...> constexpr auto always_false_v = false;
template <typename T> struct type_from_tag_constant {
    using type = T;
};
template <typename T> struct type_from_tag_constant<tag_constant<T> *> {
    using type = T;
};
template <typename> struct looking_for;
template <typename...> struct in_tuple;
template <auto> struct index;
template <auto> struct max_index;

template <typename T, typename... Ts> constexpr auto type_not_found() {
    using type = typename type_from_tag_constant<T>::type;
    static_assert(always_false_v<looking_for<type>, in_tuple<Ts...>>,
                  "Type not found in tuple!");
}

template <auto I, typename... Ts> constexpr auto index_out_of_bounds() {
    static_assert(
        always_false_v<index<I>, max_index<sizeof...(Ts) - 1>, in_tuple<Ts...>>,
        "Tuple index out of bounds!");
}
} // namespace error

namespace detail {
template <std::size_t Index, typename T, typename... Ts> struct element {
#if __has_builtin(__type_pack_element)
    using type = T;
#else
    constexpr static auto ugly_Value(index_constant<Index>) -> T;

    [[nodiscard]] constexpr auto ugly_iGet_clvr(
        index_constant<Index>) const & noexcept LIFETIMEBOUND -> T const & {
        return value;
    }
    [[nodiscard]] constexpr auto
    ugly_iGet_lvr(index_constant<Index>) & noexcept LIFETIMEBOUND -> T & {
        return value;
    }
    [[nodiscard]] constexpr auto
    ugly_iGet_rvr(index_constant<Index>) && noexcept LIFETIMEBOUND -> T && {
        return std::forward<T>(value);
    }
#endif

    template <typename U>
        requires(std::same_as<U, T> or ... or std::same_as<U, Ts>)
    [[nodiscard]] constexpr auto ugly_tGet_clvr(
        tag_constant<U> *) const & noexcept LIFETIMEBOUND -> T const & {
        return value;
    }
    template <typename U>
        requires(std::same_as<U, T> or ... or std::same_as<U, Ts>)
    [[nodiscard]] constexpr auto
    ugly_tGet_lvr(tag_constant<U> *) & noexcept LIFETIMEBOUND -> T & {
        return value;
    }
    template <typename U>
        requires(std::same_as<U, T> or ... or std::same_as<U, Ts>)
    [[nodiscard]] constexpr auto
    ugly_tGet_rvr(tag_constant<U> *) && noexcept LIFETIMEBOUND -> T && {
        return std::forward<T>(value);
    }

    constexpr auto ugly_Value_clvr() const & LIFETIMEBOUND -> T const & {
        return value;
    }
    constexpr auto ugly_Value_lvr() & LIFETIMEBOUND -> T & { return value; }
    constexpr auto ugly_Value_rvr() && LIFETIMEBOUND -> T && {
        return std::forward<T>(value);
    }

    T value;

  private:
    [[nodiscard]] friend constexpr auto
    operator==(element const &, element const &) -> bool = default;
    [[nodiscard]] friend constexpr auto operator<=>(element const &,
                                                    element const &) = default;
};

template <typename Op, typename Value> struct fold_helper {
    Op op;
    Value value;

  private:
    template <typename Rhs>
    [[nodiscard]] friend constexpr auto operator+(fold_helper &&lhs,
                                                  Rhs &&rhs) {
        using R =
            decltype(lhs.op(std::move(lhs).value, std::forward<Rhs>(rhs)));
        return fold_helper<Op, std::remove_cvref_t<R>>{
            lhs.op, lhs.op(std::move(lhs).value, std::forward<Rhs>(rhs))};
    }

    template <typename Lhs>
    [[nodiscard]] friend constexpr auto operator+(Lhs &&lhs,
                                                  fold_helper &&rhs) {
        using R =
            decltype(rhs.op(std::forward<Lhs>(lhs), std::move(rhs).value));
        return fold_helper<Op, std::remove_cvref_t<R>>{
            rhs.op, rhs.op(std::forward<Lhs>(lhs), std::move(rhs).value)};
    }
};
template <typename Op, typename Value>
fold_helper(Op, Value) -> fold_helper<Op, std::remove_cvref_t<Value>>;

template <typename Op, typename Value> struct join_helper {
    Op op;
    Value value;
};
template <typename Op, typename Value>
join_helper(Op, Value) -> join_helper<Op, std::remove_cvref_t<Value>>;

// Note: operator+ is not a hidden friend of join_helper to avoid template
// instantiation abiguity
template <typename Op, typename T, typename U>
[[nodiscard]] constexpr auto operator+(join_helper<Op, T> &&lhs,
                                       join_helper<Op, U> &&rhs) {
    using R = decltype(lhs.op(std::move(lhs).value, std::move(rhs).value));
    return join_helper<Op, std::remove_cvref_t<R>>{
        lhs.op, lhs.op(std::move(lhs).value, std::move(rhs).value)};
}

template <template <typename> typename...> struct index_function_list;
template <typename...> struct tuple_impl;

template <template <typename> typename... Fs> struct element_helper {
    template <std::size_t I, typename T>
    using element_t = element<I, T, Fs<std::remove_cvref_t<T>>...>;
};

struct index_pair {
    std::size_t outer;
    std::size_t inner;
};

template <std::size_t... Is, template <typename> typename... Fs, typename... Ts>
struct tuple_impl<std::index_sequence<Is...>, index_function_list<Fs...>, Ts...>
    : element_helper<Fs...>::template element_t<Is, Ts>... {
  private:
    template <std::size_t I, typename T>
    using base_t = typename element_helper<Fs...>::template element_t<I, T>;

  public:
    using common_tuple_comparable = void;
    using is_tuple = void;

    using base_t<Is, Ts>::ugly_tGet_clvr...;
    using base_t<Is, Ts>::ugly_tGet_lvr...;
    using base_t<Is, Ts>::ugly_tGet_rvr...;

#if __has_builtin(__type_pack_element)
    template <std::size_t I>
    using element_t = typename base_t<I, __type_pack_element<I, Ts...>>::type;
#else
    constexpr static auto ugly_Value(...) -> void;
    using base_t<Is, Ts>::ugly_Value...;
    template <std::size_t I> using element_t = decltype(ugly_Value(index<I>));

    using base_t<Is, Ts>::ugly_iGet_clvr...;
    using base_t<Is, Ts>::ugly_iGet_lvr...;
    using base_t<Is, Ts>::ugly_iGet_rvr...;
#endif

    template <typename Init, typename Op>
    [[nodiscard]] constexpr inline auto fold_left(Init &&init,
                                                  Op &&op) const & {
        return (fold_helper{op, std::forward<Init>(init)} + ... +
                this->base_t<Is, Ts>::ugly_Value_clvr())
            .value;
    }
    template <typename Init, typename Op>
    [[nodiscard]] constexpr inline auto fold_left(Init &&init, Op &&op) && {
        return (fold_helper{op, std::forward<Init>(init)} + ... +
                std::move(*this).base_t<Is, Ts>::ugly_Value_rvr())
            .value;
    }

    template <typename Init, typename Op>
    [[nodiscard]] constexpr inline auto fold_right(Init &&init,
                                                   Op &&op) const & {
        return (this->base_t<Is, Ts>::ugly_Value_clvr() + ... +
                fold_helper{op, std::forward<Init>(init)})
            .value;
    }
    template <typename Init, typename Op>
    [[nodiscard]] constexpr inline auto fold_right(Init &&init, Op &&op) && {
        return (std::move(*this).base_t<Is, Ts>::ugly_Value_rvr() + ... +
                fold_helper{op, std::forward<Init>(init)})
            .value;
    }

    template <std::size_t I>
        [[nodiscard]] constexpr auto
        operator[]([[maybe_unused]] index_constant<I> i) const
        & LIFETIMEBOUND->decltype(auto) {
        if constexpr (I >= sizeof...(Ts)) {
            error::index_out_of_bounds<I, Ts...>();
        } else {
#if __has_builtin(__type_pack_element)
            using B = base_t<I, __type_pack_element<I, Ts...>>;
            return this->B::ugly_Value_clvr();
#else
            return this->ugly_iGet_clvr(i);
#endif
        }
    }
    template <std::size_t I>
        [[nodiscard]] constexpr auto
        operator[]([[maybe_unused]] index_constant<I> i) &
        LIFETIMEBOUND->decltype(auto) {
        if constexpr (I >= sizeof...(Ts)) {
            error::index_out_of_bounds<I, Ts...>();
        } else {
#if __has_builtin(__type_pack_element)
            using B = base_t<I, __type_pack_element<I, Ts...>>;
            return this->B::ugly_Value_lvr();
#else
            return this->ugly_iGet_lvr(i);
#endif
        }
    }
    template <std::size_t I>
        [[nodiscard]] constexpr auto
        operator[]([[maybe_unused]] index_constant<I> i) &&
        LIFETIMEBOUND->decltype(auto) {
        if constexpr (I >= sizeof...(Ts)) {
            error::index_out_of_bounds<I, Ts...>();
        } else {
#if __has_builtin(__type_pack_element)
            using B [[maybe_unused]] = base_t<I, __type_pack_element<I, Ts...>>;
            return std::move(*this).B::ugly_Value_rvr();
#else
            return std::move(*this).ugly_iGet_rvr(i);
#endif
        }
    }

    constexpr auto ugly_tGet_clvr(auto idx) const & -> void {
        error::type_not_found<decltype(idx), Ts...>();
    }
    constexpr auto ugly_tGet_lvr(auto idx) & -> void {
        error::type_not_found<decltype(idx), Ts...>();
    }
    constexpr auto ugly_tGet_rvr(auto idx) && -> void {
        error::type_not_found<decltype(idx), Ts...>();
    }

    [[nodiscard]] constexpr auto get(auto idx) const & -> decltype(auto) {
        return this->ugly_tGet_clvr(idx);
    }
    [[nodiscard]] constexpr auto get(auto idx) & -> decltype(auto) {
        return this->ugly_tGet_lvr(idx);
    }
    [[nodiscard]] constexpr auto get(auto idx) && -> decltype(auto) {
        return std::move(*this).ugly_tGet_rvr(idx);
    }

    template <typename Op>
    constexpr auto apply(Op &&op) const & -> decltype(auto) {
        return std::forward<Op>(op)(this->base_t<Is, Ts>::ugly_Value_clvr()...);
    }
    template <typename Op> constexpr auto apply(Op &&op) & -> decltype(auto) {
        return std::forward<Op>(op)(this->base_t<Is, Ts>::ugly_Value_lvr()...);
    }
    template <typename Op> constexpr auto apply(Op &&op) && -> decltype(auto) {
        return std::forward<Op>(op)(
            std::move(*this).base_t<Is, Ts>::ugly_Value_rvr()...);
    }

    template <typename Op>
        requires(sizeof...(Ts) > 0)
    constexpr auto join(Op &&op) const & -> decltype(auto) {
        return (... + join_helper{op, this->base_t<Is, Ts>::ugly_Value_clvr()})
            .value;
    }
    template <typename Op>
        requires(sizeof...(Ts) > 0)
    constexpr auto join(Op &&op) && -> decltype(auto) {
        return (... +
                join_helper{op,
                            std::move(*this).base_t<Is, Ts>::ugly_Value_rvr()})
            .value;
    }

    template <typename Init, typename Op>
    constexpr auto join(Init &&init, Op &&op) const & {
        if constexpr (sizeof...(Ts) == 0) {
            return init;
        } else {
            return this->join(std::forward<Op>(op));
        }
    }
    template <typename Init, typename Op>
    constexpr auto join(Init &&init, Op &&op) && {
        if constexpr (sizeof...(Ts) == 0) {
            return init;
        } else {
            return std::move(*this).join(std::forward<Op>(op));
        }
    }

    constexpr static auto size =
        std::integral_constant<std::size_t, sizeof...(Ts)>{};

    [[nodiscard]] constexpr static auto
    fill_inner_indices(index_pair *p) -> index_pair * {
        ((p++->inner = Is), ...);
        return p;
    }
    [[nodiscard]] constexpr static auto
    fill_outer_indices(index_pair *p,
                       [[maybe_unused]] std::size_t n) -> index_pair * {
        ((p++->outer = (static_cast<void>(Is), n)), ...);
        return p;
    }

  private:
    template <typename Funcs, typename... Us>
        requires(... and std::equality_comparable_with<Ts, Us>)
    [[nodiscard]] friend constexpr auto
    operator==(tuple_impl const &lhs,
               tuple_impl<std::index_sequence<Is...>, Funcs, Us...> const &rhs)
        -> bool {
        return (... and (lhs[index<Is>] == rhs[index<Is>]));
    }

    template <typename Funcs, typename... Us>
        requires(... and std::three_way_comparable_with<Ts, Us>)
    [[nodiscard]] friend constexpr auto operator<=>(
        tuple_impl const &lhs,
        tuple_impl<std::index_sequence<Is...>, Funcs, Us...> const &rhs) {
        if constexpr (sizeof...(Is) == 0) {
            return std::strong_ordering::equal;
        } else {
            using C =
                std::common_comparison_category_t<decltype(lhs[index<Is>] <=>
                                                           rhs[index<Is>])...>;
            C result = lhs[index<0>] <=> rhs[index<0>];
            auto const compare_at = [&]<std::size_t I>() {
                result = lhs[index<I>] <=> rhs[index<I>];
                return result != 0;
            };
            [[maybe_unused]] auto b =
                (compare_at.template operator()<Is>() or ...);
            return result;
        }
    }
};

template <typename... Ts>
tuple_impl(Ts...)
    -> tuple_impl<std::index_sequence_for<Ts...>, index_function_list<>, Ts...>;
} // namespace detail

template <typename T> constexpr auto tuple_size_v = T::size();
template <typename T, std::size_t N>
constexpr auto tuple_size_v<std::array<T, N>> = N;

template <std::size_t I, typename T>
using tuple_element_t = typename T::template element_t<I>;

template <typename T>
concept tuple_comparable = requires { typename T::common_tuple_comparable; };

template <typename T>
concept tuplelike = requires { typename remove_cvref_t<T>::is_tuple; };

template <typename... Ts>
class tuple : public detail::tuple_impl<std::index_sequence_for<Ts...>,
                                        detail::index_function_list<>, Ts...> {
    template <typename U>
        requires(not tuple_comparable<U>)
    [[nodiscard]] friend constexpr auto operator==(tuple const &,
                                                   U const &) -> bool = delete;

    template <typename U>
        requires(not tuple_comparable<U>)
    [[nodiscard]] friend constexpr auto operator<=>(tuple const &,
                                                    U const &) = delete;
};
template <typename... Ts> tuple(Ts...) -> tuple<Ts...>;

template <typename IndexList, typename... Ts>
class indexed_tuple : public detail::tuple_impl<std::index_sequence_for<Ts...>,
                                                IndexList, Ts...> {
    template <typename U>
        requires(not tuple_comparable<U>)
    [[nodiscard]] friend constexpr auto operator==(indexed_tuple const &,
                                                   U const &) -> bool = delete;

    template <typename U>
        requires(not tuple_comparable<U>)
    [[nodiscard]] friend constexpr auto operator<=>(indexed_tuple const &,
                                                    U const &) = delete;
};

template <typename... Ts>
indexed_tuple(Ts...) -> indexed_tuple<detail::index_function_list<>, Ts...>;

template <std::size_t I, tuplelike Tuple>
[[nodiscard]] constexpr auto
get(Tuple &&t LIFETIMEBOUND) -> decltype(std::forward<Tuple>(t)[index<I>]) {
    return std::forward<Tuple>(t)[index<I>];
}

template <typename T, tuplelike Tuple>
[[nodiscard]] constexpr auto
get(Tuple &&t LIFETIMEBOUND) -> decltype(std::forward<Tuple>(t).get(tag<T>)) {
    return std::forward<Tuple>(t).get(tag<T>);
}

template <typename... Ts> [[nodiscard]] constexpr auto make_tuple(Ts &&...ts) {
    return tuple<std::remove_cvref_t<Ts>...>{std::forward<Ts>(ts)...};
}

template <template <typename> typename... Fs>
constexpr auto make_indexed_tuple = []<typename... Ts>(Ts &&...ts) {
    return indexed_tuple<detail::index_function_list<Fs...>,
                         std::remove_cvref_t<Ts>...>{std::forward<Ts>(ts)...};
};

template <template <typename> typename... Fs, tuplelike T>
constexpr auto apply_indices(T &&t) {
    using tuple_t = std::remove_cvref_t<T>;
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return indexed_tuple<detail::index_function_list<Fs...>,
                             tuple_element_t<Is, tuple_t>...>{
            std::forward<T>(t)[index<Is>]...};
    }(std::make_index_sequence<tuple_size_v<tuple_t>>{});
}

template <typename... Ts> constexpr auto forward_as_tuple(Ts &&...ts) {
    return stdx::tuple<Ts &&...>{std::forward<Ts>(ts)...};
}

template <typename Op, tuplelike T>
constexpr auto apply(Op &&op, T &&t) -> decltype(auto) {
    return std::forward<T>(t).apply(std::forward<Op>(op));
}

template <template <typename> typename... Fs, typename Op, tuplelike T>
constexpr auto transform(Op &&op, T &&t) {
    if constexpr (sizeof...(Fs) == 0) {
        return std::forward<T>(t).apply([&]<typename... Ts>(Ts &&...ts) {
            return stdx::tuple<decltype(op(std::forward<Ts>(ts)))...>{
                op(std::forward<Ts>(ts))...};
        });
    } else {
        return std::forward<T>(t).apply([&]<typename... Ts>(Ts &&...ts) {
            return stdx::make_indexed_tuple<Fs...>(op(std::forward<Ts>(ts))...);
        });
    }
}

template <typename Op, tuplelike T>
constexpr auto for_each(Op &&op, T &&t) -> Op {
    return std::forward<T>(t).apply([&]<typename... Ts>(Ts &&...ts) {
        (op(std::forward<Ts>(ts)), ...);
        return op;
    });
}

template <typename... Ts>
class one_of : public detail::tuple_impl<std::index_sequence_for<Ts...>,
                                         detail::index_function_list<>, Ts...> {
    template <typename T>
    constexpr friend auto operator==(one_of const &lhs, T const &rhs) -> bool {
        return lhs.apply(
            [&](auto &&...args) { return ((args == rhs) || ...); });
    }
};
template <typename... Ts> one_of(Ts...) -> one_of<Ts...>;

} // namespace v1
} // namespace stdx

#endif
