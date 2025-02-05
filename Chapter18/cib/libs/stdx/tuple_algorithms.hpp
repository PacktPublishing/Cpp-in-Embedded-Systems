#pragma once

#if __cplusplus >= 202002L

#include <stdx/ct_conversions.hpp>
#include <stdx/tuple.hpp>
#include <stdx/type_traits.hpp>

#include <boost/mp11/algorithm.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
template <typename F, tuplelike... Ts> constexpr auto apply(F &&f, Ts &&...ts) {
    constexpr auto total_num_elements =
        (std::size_t{} + ... + stdx::tuple_size_v<std::remove_cvref_t<Ts>>);

    [[maybe_unused]] constexpr auto element_indices = [&] {
        std::array<detail::index_pair, total_num_elements> indices{};
        [[maybe_unused]] auto p = indices.data();
        ((p = std::remove_cvref_t<Ts>::fill_inner_indices(p)), ...);
        [[maybe_unused]] auto q = indices.data();
        [[maybe_unused]] std::size_t n{};
        ((q = std::remove_cvref_t<Ts>::fill_outer_indices(q, n++)), ...);
        return indices;
    }();

    [[maybe_unused]] auto outer_tuple =
        stdx::tuple<Ts &&...>{std::forward<Ts>(ts)...};
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::forward<F>(f)(
            std::move(outer_tuple)[index<element_indices[Is].outer>]
                                  [index<element_indices[Is].inner>]...);
    }(std::make_index_sequence<total_num_elements>{});
}

template <tuplelike... Ts> [[nodiscard]] constexpr auto tuple_cat(Ts &&...ts) {
    if constexpr (sizeof...(Ts) == 0) {
        return stdx::tuple<>{};
    } else if constexpr (sizeof...(Ts) == 1) {
        return (ts, ...);
    } else {
        constexpr auto total_num_elements =
            (std::size_t{} + ... + stdx::tuple_size_v<std::remove_cvref_t<Ts>>);

        [[maybe_unused]] constexpr auto element_indices = [&] {
            std::array<detail::index_pair, total_num_elements> indices{};
            auto p = indices.data();
            ((p = std::remove_cvref_t<Ts>::fill_inner_indices(p)), ...);
            auto q = indices.data();
            std::size_t n{};
            ((q = std::remove_cvref_t<Ts>::fill_outer_indices(q, n++)), ...);
            return indices;
        }();

        [[maybe_unused]] auto outer_tuple =
            stdx::tuple<Ts &&...>{std::forward<Ts>(ts)...};
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            using T = stdx::tuple<stdx::tuple_element_t<
                element_indices[Is].inner,
                std::remove_cvref_t<decltype(std::move(
                    outer_tuple)[index<element_indices[Is].outer>])>>...>;
            return T{
                std::move(outer_tuple)[index<element_indices[Is].outer>]
                                      [index<element_indices[Is].inner>]...};
        }(std::make_index_sequence<total_num_elements>{});
    }
}

template <typename T, tuplelike Tup>
[[nodiscard]] constexpr auto tuple_cons(T &&t, Tup &&tup) {
    using tuple_t = std::remove_cvref_t<Tup>;
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return stdx::tuple<std::remove_cvref_t<T>,
                           stdx::tuple_element_t<Is, tuple_t>...>{
            std::forward<T>(t), std::forward<Tup>(tup)[index<Is>]...};
    }(std::make_index_sequence<stdx::tuple_size_v<tuple_t>>{});
}

template <tuplelike Tup, typename T>
[[nodiscard]] constexpr auto tuple_snoc(Tup &&tup, T &&t) {
    using tuple_t = std::remove_cvref_t<Tup>;
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return stdx::tuple<stdx::tuple_element_t<Is, tuple_t>...,
                           std::remove_cvref_t<T>>{
            std::forward<Tup>(tup)[index<Is>]..., std::forward<T>(t)};
    }(std::make_index_sequence<stdx::tuple_size_v<tuple_t>>{});
}

template <typename T, tuplelike Tup>
[[nodiscard]] constexpr auto tuple_push_front(T &&t,
                                              Tup &&tup) -> decltype(auto) {
    return tuple_cons(std::forward<T>(t), std::forward<Tup>(tup));
}

template <tuplelike Tup, typename T>
[[nodiscard]] constexpr auto tuple_push_back(Tup &&tup,
                                             T &&t) -> decltype(auto) {
    return tuple_snoc(std::forward<Tup>(tup), std::forward<T>(t));
}

template <template <typename T> typename Pred, tuplelike T>
[[nodiscard]] constexpr auto filter(T &&t) {
    using tuple_t = std::remove_cvref_t<T>;
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        constexpr auto num_matches =
            (std::size_t{} + ... +
             (Pred<stdx::tuple_element_t<Is, tuple_t>>::value ? std::size_t{1}
                                                              : std::size_t{}));
        constexpr auto indices = [] {
            auto a = std::array<std::size_t, num_matches>{};
            [[maybe_unused]] auto it = a.begin();
            [[maybe_unused]] auto copy_index =
                [&]<std::size_t I, typename Elem> {
                    if constexpr (Pred<Elem>::value) {
                        *it++ = I;
                    }
                };
            (copy_index
                 .template operator()<Is, stdx::tuple_element_t<Is, tuple_t>>(),
             ...);
            return a;
        }();

        return [&]<std::size_t... Js>(std::index_sequence<Js...>) {
            using R =
                stdx::tuple<stdx::tuple_element_t<indices[Js], tuple_t>...>;
            return R{std::forward<T>(t)[index<indices[Js]>]...};
        }(std::make_index_sequence<num_matches>{});
    }(std::make_index_sequence<stdx::tuple_size_v<tuple_t>>{});
}

namespace detail {
template <std::size_t I, typename... Ts>
constexpr auto invoke_at(auto &&op, Ts &&...ts) -> decltype(auto) {
    return op(std::forward<Ts>(ts)[index<I>]...);
}

template <typename... Ts>
constexpr std::size_t zip_length_for =
    sizeof...(Ts) == 0
        ? 0
        : std::min({stdx::tuple_size_v<std::remove_cvref_t<Ts>>...});
} // namespace detail

template <template <typename> typename... Fs, typename Op, tuplelike... Ts>
constexpr auto transform(Op &&op, Ts &&...ts) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        if constexpr (sizeof...(Fs) == 0) {
            return stdx::tuple<decltype(detail::invoke_at<Is>(
                std::forward<Op>(op), std::forward<Ts>(ts)...))...>{
                detail::invoke_at<Is>(std::forward<Op>(op),
                                      std::forward<Ts>(ts)...)...};
        } else {
            return stdx::make_indexed_tuple<Fs...>(detail::invoke_at<Is>(
                std::forward<Op>(op), std::forward<Ts>(ts)...)...);
        }
    }(std::make_index_sequence<detail::zip_length_for<Ts...>>{});
}

template <typename Op, typename... Ts>
constexpr auto unrolled_for_each(Op &&op, Ts &&...ts) -> Op {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (detail::invoke_at<Is>(op, std::forward<Ts>(ts)...), ...);
    }(std::make_index_sequence<detail::zip_length_for<Ts...>>{});
    return op;
}

template <typename Op, tuplelike... Ts>
constexpr auto for_each(Op &&op, Ts &&...ts) -> Op {
    return unrolled_for_each(std::forward<Op>(op), std::forward<Ts>(ts)...);
}

namespace detail {
template <std::size_t I, typename... Ts>
constexpr auto invoke_with_idx_at(auto &&op, Ts &&...ts) -> decltype(auto) {
    return op.template operator()<I>(std::forward<Ts>(ts)[index<I>]...);
}
} // namespace detail

template <typename Op, typename... Ts>
constexpr auto unrolled_enumerate(Op &&op, Ts &&...ts) -> Op {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (detail::invoke_with_idx_at<Is>(op, std::forward<Ts>(ts)...), ...);
    }(std::make_index_sequence<detail::zip_length_for<Ts...>>{});
    return op;
}

template <typename Op, tuplelike... Ts>
constexpr auto enumerate(Op &&op, Ts &&...ts) -> Op {
    return unrolled_enumerate(std::forward<Op>(op), std::forward<Ts>(ts)...);
}

template <typename F, tuplelike... Ts>
constexpr auto all_of(F &&f, Ts &&...ts) -> bool {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return (... and detail::invoke_at<Is>(f, std::forward<Ts>(ts)...));
    }(std::make_index_sequence<detail::zip_length_for<Ts...>>{});
}

template <typename F, tuplelike... Ts>
constexpr auto any_of(F &&f, Ts &&...ts) -> bool {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return (... or detail::invoke_at<Is>(f, std::forward<Ts>(ts)...));
    }(std::make_index_sequence<detail::zip_length_for<Ts...>>{});
}

template <typename... Ts> constexpr auto none_of(Ts &&...ts) -> bool {
    return not any_of(std::forward<Ts>(ts)...);
}

namespace detail {
template <typename T, template <typename> typename F, typename... Us>
constexpr auto is_index_for = (std::is_same_v<F<Us>, T> or ...);

template <typename T, typename IndexSeq, template <typename> typename... Fs,
          typename... Us>
constexpr auto contains_type(
    stdx::detail::tuple_impl<IndexSeq, index_function_list<Fs...>, Us...> const
        &) -> std::bool_constant<(is_index_for<T, Fs, Us...> or ...) or
                                 (std::is_same_v<T, Us> or ...)>;

template <tuplelike T, template <typename> typename Proj = std::type_identity_t>
[[nodiscard]] constexpr auto sorted_indices() {
    return []<std::size_t... Is>(std::index_sequence<Is...>)
               -> std::array<std::size_t, sizeof...(Is)> {
        using P = std::pair<std::string_view, std::size_t>;
        auto a = std::array<P, sizeof...(Is)>{
            P{stdx::type_as_string<Proj<tuple_element_t<Is, T>>>(), Is}...};
        std::sort(a.begin(), a.end(), [](auto const &p1, auto const &p2) {
            return p1.first < p2.first;
        });
        return {a[Is].second...};
    }
    (std::make_index_sequence<T::size()>{});
}
} // namespace detail

template <tuplelike Tuple, typename T>
constexpr auto contains_type =
    decltype(detail::contains_type<T>(std::declval<Tuple>()))::value;

template <template <typename> typename Proj = std::type_identity_t,
          tuplelike Tuple>
[[nodiscard]] constexpr auto sort(Tuple &&t) {
    using T = stdx::remove_cvref_t<Tuple>;
    constexpr auto indices = detail::sorted_indices<T, Proj>();
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return stdx::tuple<tuple_element_t<indices[Is], T>...>{
            std::forward<Tuple>(t)[index<indices[Is]>]...};
    }(std::make_index_sequence<T::size()>{});
}

namespace detail {

template <tuplelike T, template <typename> typename Proj> struct test_pair_t {
    template <std::size_t I, std::size_t J>
    constexpr static auto value =
        std::is_same_v<Proj<stdx::tuple_element_t<I, T>>,
                       Proj<stdx::tuple_element_t<J, T>>>;
};

template <tuplelike T, template <typename> typename Proj = std::type_identity_t>
    requires(tuple_size_v<T> > 1)
[[nodiscard]] constexpr auto count_chunks() {
    auto count = std::size_t{1};
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ((count += static_cast<std::size_t>(
              not test_pair_t<T, Proj>::template value<Is, Is + 1>)),
         ...);
    }(std::make_index_sequence<stdx::tuple_size_v<T> - 1>{});
    return count;
}

struct chunk {
    std::size_t offset{};
    std::size_t size{};
    friend constexpr auto operator==(chunk const &,
                                     chunk const &) -> bool = default;
};

template <tuplelike T, template <typename> typename Proj = std::type_identity_t>
    requires(tuple_size_v<T> > 1)
[[nodiscard]] constexpr auto create_chunks() {
    auto index = std::size_t{};
    std::array<chunk, count_chunks<T, Proj>()> chunks{};
    ++chunks[index].size;
    auto check_next_chunk = [&]<std::size_t I>() {
        if (not test_pair_t<T, Proj>::template value<I, I + 1>) {
            chunks[++index].offset = I + 1;
        }
        ++chunks[index].size;
    };

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (check_next_chunk.template operator()<Is>(), ...);
    }(std::make_index_sequence<stdx::tuple_size_v<T> - 1>{});

    return chunks;
}
} // namespace detail

template <template <typename> typename Proj = std::type_identity_t,
          tuplelike Tuple>
[[nodiscard]] constexpr auto chunk_by(Tuple &&t) {
    using tuple_t = std::remove_cvref_t<Tuple>;
    if constexpr (tuple_size_v<tuple_t> == 0) {
        return stdx::tuple{};
    } else if constexpr (tuple_size_v<tuple_t> == 1) {
        return stdx::make_tuple(std::forward<Tuple>(t));
    } else {
        constexpr auto chunks = detail::create_chunks<tuple_t, Proj>();
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return stdx::make_tuple(
                [&]<std::size_t... Js>(std::index_sequence<Js...>) {
                    constexpr auto offset = chunks[Is].offset;
                    return stdx::tuple<tuple_element_t<
                        offset + Js, stdx::remove_cvref_t<Tuple>>...>{
                        std::forward<Tuple>(t)[index<offset + Js>]...};
                }(std::make_index_sequence<chunks[Is].size>{})...);
        }(std::make_index_sequence<chunks.size()>{});
    }
}

template <tuplelike Tuple> [[nodiscard]] constexpr auto chunk(Tuple &&t) {
    return chunk_by(std::forward<Tuple>(t));
}

template <tuplelike... Ts> constexpr auto cartesian_product_copy(Ts &&...ts) {
    if constexpr (sizeof...(Ts) == 0) {
        return make_tuple(tuple{});
    } else {
        return []<typename First, typename... Rest>(First &&first,
                                                    Rest &&...rest) {
            auto const c = cartesian_product_copy(std::forward<Rest>(rest)...);
            return std::forward<First>(first).apply([&]<typename... Elems>(
                                                        Elems &&...elems) {
                [[maybe_unused]] auto const prepend = [&]<typename E>(E &&e) {
                    return c.apply([&](auto... subs) {
                        return make_tuple(
                            tuple_cat(make_tuple(std::forward<E>(e)), subs)...);
                    });
                };
                return tuple_cat(prepend(std::forward<Elems>(elems))...);
            });
        }(std::forward<Ts>(ts)...);
    }
}

template <tuplelike... Ts> constexpr auto cartesian_product(Ts &&...ts) {
    if constexpr (sizeof...(Ts) == 0) {
        return make_tuple(tuple{});
    } else {
        return []<typename First, typename... Rest>(First &&first,
                                                    Rest &&...rest) {
            auto const c = cartesian_product(std::forward<Rest>(rest)...);
            return std::forward<First>(first).apply(
                [&]<typename... Elems>(Elems &&...elems) {
                    auto const prepend = [&]<typename E>(E &&e) {
                        return c.apply([&](auto... subs) {
                            return make_tuple(tuple_cat(
                                forward_as_tuple(std::forward<E>(e)), subs)...);
                        });
                    };
                    return tuple_cat(prepend(std::forward<Elems>(elems))...);
                });
        }(std::forward<Ts>(ts)...);
    }
}

template <tuplelike T> constexpr auto unique(T &&t) {
    return chunk(std::forward<T>(t)).apply([]<typename... Us>(Us &&...us) {
        return tuple<tuple_element_t<0, Us>...>{
            get<0>(std::forward<Us>(us))...};
    });
}

template <tuplelike T> constexpr auto to_sorted_set(T &&t) {
    return unique(sort(std::forward<T>(t)));
}

template <tuplelike Tuple> constexpr auto to_unsorted_set(Tuple &&t) {
    using T = stdx::remove_cvref_t<Tuple>;
    using U = boost::mp11::mp_unique<T>;
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return U{get<boost::mp11::mp_find<T, tuple_element_t<Is, U>>::value>(
            std::forward<Tuple>(t))...};
    }(std::make_index_sequence<U::size()>{});
}

template <template <typename> typename Proj = std::type_identity_t,
          tuplelike Tuple>
[[nodiscard]] constexpr auto gather_by(Tuple &&t) {
    using tuple_t = std::remove_cvref_t<Tuple>;
    if constexpr (tuple_size_v<tuple_t> == 0) {
        return stdx::tuple{};
    } else if constexpr (tuple_size_v<tuple_t> == 1) {
        return stdx::make_tuple(std::forward<Tuple>(t));
    } else {
        constexpr auto sorted_idxs = detail::sorted_indices<tuple_t, Proj>();
        constexpr auto tests =
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::array<bool, stdx::tuple_size_v<tuple_t> - 1>{
                    detail::test_pair_t<tuple_t, Proj>::template value<
                        sorted_idxs[Is], sorted_idxs[Is + 1]>...};
            }(std::make_index_sequence<stdx::tuple_size_v<tuple_t> - 1>{});

        constexpr auto chunks = [&] {
            constexpr auto chunk_count =
                std::count(std::begin(tests), std::end(tests), false) + 1;
            std::array<detail::chunk, chunk_count> cs{};

            auto index = std::size_t{};
            ++cs[index].size;
            for (auto i = std::size_t{}; i < std::size(tests); ++i) {
                if (not tests[i]) {
                    cs[++index].offset = i + 1;
                }
                ++cs[index].size;
            }
            return cs;
        }();

        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return stdx::make_tuple([&]<std::size_t... Js>(
                                        std::index_sequence<Js...>) {
                constexpr auto offset = chunks[Is].offset;
                return stdx::tuple<
                    tuple_element_t<sorted_idxs[offset + Js], tuple_t>...>{
                    std::forward<Tuple>(t)[index<sorted_idxs[offset + Js]>]...};
            }(std::make_index_sequence<chunks[Is].size>{})...);
        }(std::make_index_sequence<chunks.size()>{});
    }
}

template <tuplelike Tuple> [[nodiscard]] constexpr auto gather(Tuple &&t) {
    return gather_by(std::forward<Tuple>(t));
}
} // namespace v1
} // namespace stdx

#endif
