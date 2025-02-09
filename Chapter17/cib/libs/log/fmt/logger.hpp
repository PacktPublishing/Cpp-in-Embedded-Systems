#pragma once

#include <log/level.hpp>
#include <log/log.hpp>
#include <log/module.hpp>

#include <stdx/ct_format.hpp>
#include <stdx/tuple.hpp>
#include <stdx/tuple_algorithms.hpp>
#include <stdx/utility.hpp>

#include <fmt/format.h>

#include <chrono>
#include <type_traits>
#include <utility>

template <logging::level L>
struct fmt::formatter<std::integral_constant<logging::level, L>> {
    constexpr static auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(std::integral_constant<logging::level, L>,
                FormatContext &ctx) const {
        return ::fmt::format_to(ctx.out(), logging::to_text<L>());
    }
};

namespace logging::fmt {
template <typename TDestinations> struct log_handler {
    constexpr explicit log_handler(TDestinations &&ds) : dests{std::move(ds)} {}

    template <typename Env, typename FilenameStringType,
              typename LineNumberType, typename MsgType>
    auto log(FilenameStringType, LineNumberType, MsgType const &msg) -> void {
        auto const currentTime =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start_time)
                .count();

        stdx::for_each(
            [&](auto &out) {
                ::fmt::format_to(out, "{:>8}us {} [{}]: ", currentTime,
                                 get_level(Env{}), get_module(Env{}).value);
                msg.apply(
                    [&]<typename StringType>(StringType, auto const &...args) {
                        ::fmt::format_to(out, StringType::value, args...);
                    });
                *out = '\n';
            },
            dests);
    }

  private:
    static inline auto const start_time = std::chrono::steady_clock::now();
    TDestinations dests;
};

template <typename... TDestinations> struct config {
    using destinations_tuple_t = stdx::tuple<TDestinations...>;
    constexpr explicit config(TDestinations... dests)
        : logger{stdx::tuple{std::move(dests)...}} {}

    log_handler<destinations_tuple_t> logger;
};
} // namespace logging::fmt
