#pragma once

#include <conc/atomic.hpp>

#include <stdx/bit.hpp>
#include <stdx/bitset.hpp>
#include <stdx/compiler.hpp>
#include <stdx/concepts.hpp>
#include <stdx/ct_string.hpp>
#include <stdx/detail/bitset_common.hpp>
#include <stdx/type_traits.hpp>
#include <stdx/udls.hpp>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iterator>
#include <limits>
#include <string_view>

namespace stdx {
inline namespace v1 {
template <auto Size,
          typename StorageElem = decltype(smallest_uint<to_underlying(Size)>())>
class atomic_bitset {
    constexpr static std::size_t N = to_underlying(Size);

    using elem_t = atomic::atomic_type_t<StorageElem>;
    constexpr static auto alignment = atomic::alignment_of<StorageElem>;

    static_assert(std::is_unsigned_v<elem_t>,
                  "Storage element for atomic_bitset must be an unsigned type");

    constexpr static auto bit = elem_t{1U};

    static_assert(N <= std::numeric_limits<elem_t>::digits,
                  "atomic_bitset is limited to a single storage element");
    alignas(alignment) elem_t storage{};

    constexpr static auto mask = bit_mask<elem_t, N - 1>();
    auto salient_value(std::memory_order order) const -> elem_t {
        return atomic::load(storage, order) & mask;
    }

    [[nodiscard]] static constexpr auto value_from_string(std::string_view str,
                                                          std::size_t pos,
                                                          std::size_t n,
                                                          char one) -> elem_t {
        elem_t ret{};
        auto const len = std::min(n, str.size() - pos);
        auto const s = str.substr(pos, std::min(len, N));
        auto i = bit;
        // NOLINTNEXTLINE(modernize-loop-convert)
        for (auto it = std::rbegin(s); it != std::rend(s); ++it) {
            if (*it == one) {
                ret |= i;
            }
            i = static_cast<elem_t>(i << 1u);
        }
        return ret;
    }

    using bitset_t = bitset<Size, elem_t>;

  public:
    constexpr atomic_bitset() = default;
    constexpr explicit atomic_bitset(std::uint64_t value)
        : storage{static_cast<elem_t>(value & mask)} {}

    template <typename... Bs>
    constexpr explicit atomic_bitset(place_bits_t, Bs... bs)
        : storage{static_cast<elem_t>(
              (elem_t{} | ... |
               static_cast<elem_t>(bit << to_underlying(bs))))} {}

    constexpr explicit atomic_bitset(all_bits_t) : storage{mask} {}

    constexpr explicit atomic_bitset(std::string_view str, std::size_t pos = 0,
                                     std::size_t n = std::string_view::npos,
                                     char one = '1')
        : storage{value_from_string(str, pos, n, one)} {}

#if __cplusplus >= 202002L
    constexpr explicit atomic_bitset(ct_string<N + 1> s)
        : atomic_bitset{static_cast<std::string_view>(s)} {}
#endif

    template <typename T>
    [[nodiscard]] auto
    to(std::memory_order order = std::memory_order_seq_cst) const -> T {
        using U = underlying_type_t<T>;
        static_assert(
            unsigned_integral<U>,
            "Conversion must be to an unsigned integral type or enum!");
        static_assert(N <= std::numeric_limits<U>::digits,
                      "atomic_bitset must fit within T");
        return static_cast<T>(salient_value(order));
    }

    [[nodiscard]] auto
    to_natural(std::memory_order order = std::memory_order_seq_cst) const
        -> StorageElem {
        return static_cast<StorageElem>(salient_value(order));
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    operator bitset_t() const {
        return bitset_t{salient_value(std::memory_order_seq_cst)};
    }

    auto load(std::memory_order order = std::memory_order_seq_cst) const
        -> bitset_t {
        return bitset_t{salient_value(order)};
    }
    auto store(bitset_t b,
               std::memory_order order = std::memory_order_seq_cst) {
        atomic::store(storage, b.template to<elem_t>(), order);
    }

    constexpr static std::integral_constant<std::size_t, N> size{};

    template <typename T> [[nodiscard]] auto operator[](T idx) const -> bool {
        return load()[idx];
    }

    template <typename T>
    auto set(T idx, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        if (value) {
            return bitset_t{atomic::fetch_or(
                storage, static_cast<elem_t>(bit << pos), order)};
        }
        return bitset_t{atomic::fetch_and(
            storage, static_cast<elem_t>(~(bit << pos)), order)};
    }

    auto set(lsb_t lsb, msb_t msb, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const m = to_underlying(msb);
        auto const shifted_value = bit_mask<elem_t>(m, l);
        if (value) {
            return bitset_t{atomic::fetch_or(storage, shifted_value, order)};
        }
        return bitset_t{atomic::fetch_and(storage, ~shifted_value, order)};
    }

    auto set(lsb_t lsb, length_t len, bool value = true,
             std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const length = to_underlying(len);
        return set(lsb, static_cast<msb_t>(l + length - 1), value, order);
    }

    auto set(std::memory_order order = std::memory_order_seq_cst)
        LIFETIMEBOUND -> atomic_bitset & {
        atomic::store(storage, mask, order);
        return *this;
    }

    template <typename T>
    auto reset(T idx, std::memory_order order = std::memory_order_seq_cst)
        -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        return bitset_t{atomic::fetch_and(
            storage, static_cast<elem_t>(~(bit << pos)), order)};
    }

    auto
    reset(lsb_t lsb, msb_t msb,
          std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const m = to_underlying(msb);
        auto const shifted_value = bit_mask<elem_t>(m, l);
        return bitset_t{atomic::fetch_and(storage, ~shifted_value, order)};
    }

    auto
    reset(lsb_t lsb, length_t len,
          std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const length = to_underlying(len);
        return reset(lsb, static_cast<msb_t>(l + length - 1), order);
    }

    auto reset(std::memory_order order = std::memory_order_seq_cst)
        LIFETIMEBOUND -> atomic_bitset & {
        atomic::store(storage, elem_t{}, order);
        return *this;
    }

    template <typename T>
    auto flip(T idx,
              std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const pos = static_cast<std::size_t>(to_underlying(idx));
        return bitset_t{
            atomic::fetch_xor(storage, static_cast<elem_t>(bit << pos), order)};
    }

    auto flip(lsb_t lsb, msb_t msb,
              std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const m = to_underlying(msb);
        auto const shifted_value = bit_mask<elem_t>(m, l);
        return bitset_t{atomic::fetch_xor(storage, shifted_value, order)};
    }

    auto flip(lsb_t lsb, length_t len,
              std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        auto const l = to_underlying(lsb);
        auto const length = to_underlying(len);
        return flip(lsb, static_cast<msb_t>(l + length - 1), order);
    }

    auto flip(std::memory_order order = std::memory_order_seq_cst) -> bitset_t {
        return bitset_t{atomic::fetch_xor(storage, mask, order)};
    }

    [[nodiscard]] auto
    all(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) == mask;
    }
    [[nodiscard]] auto
    any(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) != 0;
    }
    [[nodiscard]] auto
    none(std::memory_order order = std::memory_order_seq_cst) const -> bool {
        return salient_value(order) == 0;
    }

    [[nodiscard]] auto
    count(std::memory_order order = std::memory_order_seq_cst) const
        -> std::size_t {
        return static_cast<std::size_t>(popcount(salient_value(order)));
    }
};

#if __cplusplus >= 202002L
template <std::size_t N> atomic_bitset(ct_string<N>) -> atomic_bitset<N - 1>;
#endif
} // namespace v1
} // namespace stdx
