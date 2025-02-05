#pragma once

#include <stdx/bit.hpp>
#include <stdx/compiler.hpp>
#include <stdx/iterator.hpp>
#include <stdx/memory.hpp>
#include <stdx/type_traits.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {

constexpr static auto dynamic_extent = std::numeric_limits<std::size_t>::max();

namespace detail {
template <typename T, std::size_t N> struct span_base {
    constexpr span_base() = default;
    template <typename It, typename SizeOrEnd>
    constexpr explicit span_base(It, SizeOrEnd) {}

    constexpr static std::integral_constant<std::size_t, N> size{};
    constexpr static std::integral_constant<std::size_t, N * sizeof(T)>
        size_bytes{};
    constexpr static std::bool_constant<N == 0> empty{};
};

template <typename T> class span_base<T, dynamic_extent> {
    std::size_t sz{};

  public:
    constexpr span_base() = default;

    template <typename It, typename SizeOrEnd,
              std::enable_if_t<std::is_integral_v<SizeOrEnd>, int> = 0>
    constexpr span_base(It, SizeOrEnd count)
        : sz{static_cast<std::size_t>(count)} {}

    template <typename It, typename SizeOrEnd,
              std::enable_if_t<not std::is_integral_v<SizeOrEnd>, int> = 0>
    constexpr span_base(It first, SizeOrEnd last)
        : sz{static_cast<std::size_t>(std::distance(first, last))} {}

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
        return sz;
    }
    [[nodiscard]] constexpr auto size_bytes() const noexcept -> std::size_t {
        return sz * sizeof(T);
    }
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return sz == 0u;
    }
};
} // namespace detail

template <typename T, std::size_t Extent = dynamic_extent>
class span : public detail::span_base<T, Extent> {
    template <typename> constexpr static inline auto dependent_extent = Extent;
    using base_t = detail::span_base<T, Extent>;

    T *ptr{};

  public:
    using element_type = T;
    using value_type = stdx::remove_cvref_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr static inline auto extent = Extent;

    constexpr span() = default;

    template <typename It, typename SizeOrEnd,
              std::enable_if_t<dependent_extent<It> != dynamic_extent, int> = 0>
    explicit constexpr span(It first, SizeOrEnd)
        : ptr{stdx::to_address(first)} {}

    template <typename It, typename SizeOrEnd,
              std::enable_if_t<dependent_extent<It> == dynamic_extent, int> = 0>
    constexpr span(It first, SizeOrEnd sore)
        : base_t{first, sore}, ptr{stdx::to_address(first)} {}

    template <typename U, std::size_t N>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(std::array<U, N> &arr LIFETIMEBOUND) noexcept
        : ptr{std::data(arr)} {
        static_assert(Extent == dynamic_extent or Extent <= N,
                      "Span extends beyond available storage");
    }

    template <typename U, std::size_t N>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(std::array<U, N> const &arr LIFETIMEBOUND) noexcept
        : ptr{std::data(arr)} {
        static_assert(Extent == dynamic_extent or Extent <= N,
                      "Span extends beyond available storage");
    }

    template <std::size_t N>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        stdx::type_identity_t<element_type> (&arr)[N] LIFETIMEBOUND) noexcept
        : ptr{std::data(arr)} {
        static_assert(Extent == dynamic_extent or Extent <= N,
                      "Span extends beyond available storage");
    }

    template <typename R,
              std::enable_if_t<dependent_extent<R> != dynamic_extent, int> = 0>
    explicit constexpr span(R &&r)
        : ptr{stdx::to_address(std::begin(std::forward<R>(r)))} {}

    template <typename R,
              std::enable_if_t<dependent_extent<R> == dynamic_extent, int> = 0>
    explicit constexpr span(R &&r)
        : base_t{std::begin(std::forward<R>(r)), std::end(std::forward<R>(r))},
          ptr{stdx::to_address(std::begin(std::forward<R>(r)))} {}

    template <class U, std::size_t N,
              std::enable_if_t<dependent_extent<U> != dynamic_extent and
                                   N == dynamic_extent,
                               int> = 0>
    explicit constexpr span(span<U, N> const &s) noexcept : ptr{s.data()} {}

    template <class U, std::size_t N,
              std::enable_if_t<dependent_extent<U> == dynamic_extent or
                                   N != dynamic_extent,
                               int> = 0>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr span(span<U, N> const &s) noexcept
        : base_t{s.data(), s.size()}, ptr{s.data()} {}

    [[nodiscard]] constexpr auto data() const noexcept -> pointer {
        return ptr;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator {
        return ptr;
    }
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return ptr;
    }
    [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator {
        return std::reverse_iterator{end()};
    }
    [[nodiscard]] constexpr auto
    crbegin() const noexcept -> const_reverse_iterator {
        return std::reverse_iterator{cend()};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator {
        return ptr + this->size();
    }
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return ptr + this->size();
    }
    [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator {
        return std::reverse_iterator{begin()};
    }
    [[nodiscard]] constexpr auto
    crend() const noexcept -> const_reverse_iterator {
        return std::reverse_iterator{cbegin()};
    }

    [[nodiscard]] constexpr auto front() const -> reference { return *begin(); }
    [[nodiscard]] constexpr auto back() const -> reference {
        return *std::prev(end());
    }

    [[nodiscard]] constexpr auto operator[](size_type idx) const -> reference {
        return data()[idx];
    }

    template <std::size_t Count>
    [[nodiscard]] constexpr auto first() const -> span<element_type, Count> {
        static_assert(Count <= Extent, "first cannot form a larger span!");
        return span<element_type, Count>{ptr, Count};
    }
    [[nodiscard]] constexpr auto
    first(size_type count) const -> span<element_type, dynamic_extent> {
        return {ptr, count};
    }

    template <std::size_t Count>
    [[nodiscard]] constexpr auto last() const -> span<element_type, Count> {
        static_assert(Count <= Extent, "last cannot form a larger span!");
        return span<element_type, Count>{ptr + this->size() - Count, Count};
    }
    [[nodiscard]] constexpr auto
    last(size_type count) const -> span<element_type, dynamic_extent> {
        return {ptr + this->size() - count, count};
    }

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    [[nodiscard]] constexpr auto subspan() const {
        if constexpr (Count != dynamic_extent) {
            static_assert(Offset <= Extent,
                          "subspan cannot start beyond span!");
            static_assert(Count <= Extent,
                          "subspan cannot be longer than span!");
            static_assert(Offset <= Extent - Count,
                          "subspan cannot end beyond span!");
            return span<element_type, Count>{ptr + Offset, Count};
        } else if constexpr (Extent != dynamic_extent) {
            static_assert(Offset <= Extent,
                          "subspan cannot start beyond span!");
            return span<element_type, Extent - Offset>{ptr + Offset,
                                                       Extent - Offset};
        } else {
            return span<element_type, dynamic_extent>{ptr + Offset,
                                                      this->size() - Offset};
        }
    }

    [[nodiscard]] constexpr auto subspan(size_type Offset,
                                         size_type Count = dynamic_extent) const
        -> span<element_type, dynamic_extent> {
        return {ptr + Offset, std::min(Count, this->size() - Offset)};
    }
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
template <class T, std::size_t N> auto as_bytes(span<T, N> s) noexcept {
    if constexpr (N == dynamic_extent) {
        return span{reinterpret_cast<std::byte const *>(s.data()),
                    s.size_bytes()};
    } else {
        constexpr auto size = s.size_bytes();
        return span<std::byte const, size>{
            reinterpret_cast<std::byte const *>(s.data()), size};
    }
}

template <class T, std::size_t N,
          std::enable_if_t<not std::is_const_v<T>, int> = 0>
auto as_writable_bytes(span<T, N> s) noexcept {
    if constexpr (N == dynamic_extent) {
        return span{reinterpret_cast<std::byte *>(s.data()), s.size_bytes()};
    } else {
        constexpr auto size = s.size_bytes();
        return span<std::byte, size>{reinterpret_cast<std::byte *>(s.data()),
                                     size};
    }
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

namespace detail {
template <typename T> using iter_reference_t = decltype(*std::declval<T &>());
template <typename T>
using iterator_t = decltype(std::begin(std::declval<T &>()));
template <typename R> using range_reference_t = iter_reference_t<iterator_t<R>>;
} // namespace detail

template <typename It, typename EndOrSize>
span(It,
     EndOrSize) -> span<std::remove_reference_t<detail::iter_reference_t<It>>>;
// NOLINTNEXTLINE(*-avoid-c-arrays)
template <typename T, std::size_t N> span(T (&)[N]) -> span<T, N>;
template <typename T, std::size_t N> span(std::array<T, N> &) -> span<T, N>;
template <typename T, std::size_t N>
span(std::array<T, N> const &) -> span<T const, N>;
template <typename R>
span(R &&) -> span<std::remove_reference_t<detail::range_reference_t<R>>>;

template <typename T, std::size_t N>
constexpr auto ct_capacity_v<span<T, N>> = N;
} // namespace v1
} // namespace stdx

// NOLINTEND(modernize-use-constraints)
