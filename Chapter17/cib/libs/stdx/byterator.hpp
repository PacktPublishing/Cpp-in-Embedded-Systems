#pragma once

#include <stdx/bit.hpp>
#include <stdx/memory.hpp>
#include <stdx/utility.hpp>

#include <cstddef>
#include <cstring>
#include <functional>
#include <iterator>
#include <type_traits>

// NOLINTBEGIN(modernize-use-constraints)

namespace stdx {
inline namespace v1 {

namespace detail {
template <typename It>
constexpr auto is_byteratorish_v =
    std::is_base_of_v<std::random_access_iterator_tag,
                      typename std::iterator_traits<It>::iterator_category> and
    std::is_trivially_copyable_v<typename std::iterator_traits<It>::value_type>;

template <typename It>
constexpr auto iterator_value_type()
    -> decltype(*std::declval<typename std::iterator_traits<It>::pointer>());

template <typename It>
using iterator_value_t = decltype(iterator_value_type<It>());
} // namespace detail

template <typename T> class byterator {
    using byte_t = std::remove_reference_t<forward_like_t<T, std::byte>>;
    byte_t *ptr;

    [[nodiscard]] constexpr friend auto operator==(byterator const &x,
                                                   byterator const &y) -> bool {
        return x.ptr == y.ptr;
    }

    template <typename It,
              std::enable_if_t<std::is_same_v<detail::iterator_value_t<It>, T>,
                               int> = 0>
    [[nodiscard]] constexpr friend auto operator==(byterator const &x,
                                                   It y) -> bool {
        return static_cast<void const *>(x.ptr) ==
               static_cast<void const *>(stdx::to_address(y));
    }

#if __cpp_impl_three_way_comparison >= 201907L
    [[nodiscard]] constexpr friend auto operator<=>(byterator const &x,
                                                    byterator const &y) {
        return x.ptr <=> y.ptr;
    }
#else
    [[nodiscard]] constexpr friend auto operator!=(byterator const &x,
                                                   byterator const &y) -> bool {
        return not(x == y);
    }

    template <typename It>
    [[nodiscard]] constexpr friend auto operator==(It y,
                                                   byterator const &x) -> bool {
        return x == y;
    }
    template <typename It>
    [[nodiscard]] constexpr friend auto operator!=(byterator const &x,
                                                   It y) -> bool {
        return not(x == y);
    }
    template <typename It>
    [[nodiscard]] constexpr friend auto operator!=(It y,
                                                   byterator const &x) -> bool {
        return not(x == y);
    }

    [[nodiscard]] constexpr friend auto operator<(byterator const &x,
                                                  byterator const &y) -> bool {
        return std::less{}(x.ptr, y.ptr);
    }
    [[nodiscard]] constexpr friend auto operator<=(byterator const &x,
                                                   byterator const &y) -> bool {
        return std::less_equal{}(x.ptr, y.ptr);
    }
    [[nodiscard]] constexpr friend auto operator>(byterator const &x,
                                                  byterator const &y) -> bool {
        return std::greater{}(x.ptr, y.ptr);
    }
    [[nodiscard]] constexpr friend auto operator>=(byterator const &x,
                                                   byterator const &y) -> bool {
        return std::greater_equal{}(x.ptr, y.ptr);
    }
#endif

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::byte;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::random_access_iterator_tag;

    template <typename It,
              std::enable_if_t<detail::is_byteratorish_v<It>, int> = 0>
    explicit byterator(It it) : ptr(bit_cast<byte_t *>(stdx::to_address(it))) {}

    [[nodiscard]] constexpr auto operator->() const -> byte_t * { return ptr; }
    [[nodiscard]] constexpr auto operator*() const -> byte_t & { return *ptr; }

    constexpr auto operator++() -> byterator & {
        ++ptr;
        return *this;
    }
    [[nodiscard]] constexpr auto operator++(int) -> byterator {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
    constexpr auto operator--() -> byterator & {
        --ptr;
        return *this;
    }
    [[nodiscard]] constexpr auto operator--(int) -> byterator {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    constexpr auto operator+=(difference_type d) -> byterator & {
        ptr += d;
        return *this;
    }
    constexpr auto operator-=(difference_type d) -> byterator & {
        ptr -= d;
        return *this;
    }

    [[nodiscard]] friend constexpr auto
    operator+(byterator i, difference_type d) -> byterator {
        i += d;
        return i;
    }
    [[nodiscard]] friend constexpr auto operator+(difference_type d,
                                                  byterator i) -> byterator {
        i += d;
        return i;
    }
    [[nodiscard]] friend constexpr auto
    operator-(byterator i, difference_type d) -> byterator {
        i -= d;
        return i;
    }
    [[nodiscard]] friend constexpr auto
    operator-(byterator x, byterator y) -> difference_type {
        return x.ptr - y.ptr;
    }

    [[nodiscard]] constexpr auto operator[](difference_type n) -> byte_t & {
        return ptr[n];
    }
    [[nodiscard]] constexpr auto
    operator[](difference_type n) const -> byte_t const & {
        return ptr[n];
    }

    template <typename V = std::uint8_t, typename R = V,
              std::enable_if_t<std::is_trivially_copyable_v<V>, int> = 0>
    [[nodiscard]] auto peek() -> R {
        V v;
        std::memcpy(std::addressof(v), ptr, sizeof(V));
        return static_cast<R>(v);
    }

    template <typename V = std::uint8_t, typename R = V,
              std::enable_if_t<std::is_trivially_copyable_v<V>, int> = 0>
    [[nodiscard]] auto read() -> R {
        R ret = peek<V, R>();
        ptr += sizeof(V);
        return ret;
    }

    template <typename V,
              std::enable_if_t<std::is_trivially_copyable_v<remove_cvref_t<V>>,
                               int> = 0>
    auto write(V &&v) -> void {
        using R = remove_cvref_t<V>;
        std::memcpy(ptr, std::addressof(v), sizeof(R));
        ptr += sizeof(R);
    }

    template <typename V = std::uint8_t> [[nodiscard]] auto peeku8() {
        return peek<std::uint8_t, V>();
    }
    template <typename V = std::uint8_t> [[nodiscard]] auto readu8() {
        return read<std::uint8_t, V>();
    }
    template <typename V> [[nodiscard]] auto writeu8(V &&v) {
        return write(static_cast<std::uint8_t>(std::forward<V>(v)));
    }

    template <typename V = std::uint16_t> [[nodiscard]] auto peeku16() {
        return peek<std::uint16_t, V>();
    }
    template <typename V = std::uint16_t> [[nodiscard]] auto readu16() {
        return read<std::uint16_t, V>();
    }
    template <typename V> [[nodiscard]] auto writeu16(V &&v) {
        return write(static_cast<std::uint16_t>(std::forward<V>(v)));
    }

    template <typename V = std::uint32_t> [[nodiscard]] auto peeku32() {
        return peek<std::uint32_t, V>();
    }
    template <typename V = std::uint32_t> [[nodiscard]] auto readu32() {
        return read<std::uint32_t, V>();
    }
    template <typename V> [[nodiscard]] auto writeu32(V &&v) {
        return write(static_cast<std::uint32_t>(std::forward<V>(v)));
    }
};

template <typename It, std::enable_if_t<detail::is_byteratorish_v<It>, int> = 0>
byterator(It) -> byterator<detail::iterator_value_t<It>>;

} // namespace v1
} // namespace stdx

// NOLINTEND(modernize-use-constraints)
