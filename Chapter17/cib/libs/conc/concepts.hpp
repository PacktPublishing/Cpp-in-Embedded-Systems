#pragma once

#if __cplusplus >= 202002L

#include <atomic>
#include <concepts>

namespace conc {
template <typename T>
concept policy = requires(auto (*f)()->int &&, auto (*pred)()->bool) {
    { T::call_in_critical_section(f) } -> std::same_as<int &&>;
    { T::call_in_critical_section(f, pred) } -> std::same_as<int &&>;
};
} // namespace conc

namespace atomic {
template <typename T>
concept load_store_policy = requires(int &a, int value, std::memory_order mo) {
    { T::load(a) } -> std::same_as<int>;
    { T::load(a, mo) } -> std::same_as<int>;
    { T::store(a, value) } -> std::same_as<void>;
    { T::store(a, value, mo) } -> std::same_as<void>;
};

template <typename T>
concept exchange_policy =
    load_store_policy<T> and requires(int &a, int value, std::memory_order mo) {
        { T::exchange(a, value) } -> std::same_as<int>;
        { T::exchange(a, value, mo) } -> std::same_as<int>;
    };

template <typename T>
concept add_sub_policy =
    load_store_policy<T> and requires(int &a, int value, std::memory_order mo) {
        { T::fetch_add(a, value) } -> std::same_as<int>;
        { T::fetch_add(a, value, mo) } -> std::same_as<int>;
        { T::fetch_sub(a, value) } -> std::same_as<int>;
        { T::fetch_sub(a, value, mo) } -> std::same_as<int>;
    };

template <typename T>
concept bitwise_policy =
    load_store_policy<T> and requires(int &a, int value, std::memory_order mo) {
        { T::fetch_and(a, value) } -> std::same_as<int>;
        { T::fetch_and(a, value, mo) } -> std::same_as<int>;
        { T::fetch_or(a, value) } -> std::same_as<int>;
        { T::fetch_or(a, value, mo) } -> std::same_as<int>;
        { T::fetch_xor(a, value) } -> std::same_as<int>;
        { T::fetch_xor(a, value, mo) } -> std::same_as<int>;
    };

template <typename T>
concept policy = exchange_policy<T> and add_sub_policy<T> and bitwise_policy<T>;
} // namespace atomic

#endif
