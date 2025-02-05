#pragma once

#include <stdx/compiler.hpp>
#include <stdx/functional.hpp>
#include <stdx/type_traits.hpp>

#include <optional>
#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {
template <typename F> struct cached {
    using value_type = stdx::remove_cvref_t<std::invoke_result_t<F>>;

    constexpr explicit cached(F const &f) : lazy{f} {}
    constexpr explicit cached(F &&f) : lazy{std::move(f)} {}

    constexpr auto has_value() const noexcept -> bool {
        return opt.has_value();
    }
    constexpr explicit operator bool() const noexcept {
        return opt.has_value();
    }

    constexpr auto value() & LIFETIMEBOUND -> value_type & {
        populate();
        return *opt;
    }
    constexpr auto value() const & LIFETIMEBOUND -> value_type const & {
        populate();
        return *opt;
    }
    constexpr auto value() && LIFETIMEBOUND -> value_type && {
        populate();
        return *std::move(opt);
    }
    constexpr auto value() const && LIFETIMEBOUND -> value_type const && {
        populate();
        return *std::move(opt);
    }

    constexpr auto operator->() const LIFETIMEBOUND->value_type const * {
        populate();
        return opt.operator->();
    }
    constexpr auto operator->() LIFETIMEBOUND->value_type * {
        populate();
        return opt.operator->();
    }

    constexpr auto operator*() const & LIFETIMEBOUND->decltype(auto) {
        return value();
    }
    constexpr auto operator*() & LIFETIMEBOUND->decltype(auto) {
        return value();
    }
    constexpr auto operator*() const && LIFETIMEBOUND->decltype(auto) {
        return std::move(*this).value();
    }
    constexpr auto operator*() && LIFETIMEBOUND->decltype(auto) {
        return std::move(*this).value();
    }

    auto reset() { opt.reset(); }
    auto refresh() LIFETIMEBOUND -> value_type & {
        opt.reset();
        populate();
        return *opt;
    }

  private:
    constexpr auto populate() const {
        if (not opt.has_value()) {
            opt.emplace(lazy);
        }
    }

    with_result_of<F> lazy;
    mutable std::optional<value_type> opt{};
};

template <typename C>
using cached_value_t = typename stdx::remove_cvref_t<C>::value_type;
} // namespace v1
} // namespace stdx
