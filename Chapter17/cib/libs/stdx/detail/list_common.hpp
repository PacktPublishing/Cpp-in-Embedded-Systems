#pragma once

#include <stdx/concepts.hpp>
#include <stdx/panic.hpp>

#include <type_traits>
#include <utility>

namespace stdx {
inline namespace v1 {

#if __cpp_concepts >= 201907L
namespace detail {
template <typename T>
concept base_single_linkable = requires(T node) {
    { node->next } -> same_as<T &>;
};

template <typename T>
concept base_double_linkable = base_single_linkable<T> and requires(T node) {
    { node->prev } -> same_as<T &>;
};
} // namespace detail

template <typename T>
concept single_linkable = requires(T *node) {
    requires detail::base_single_linkable<
        std::remove_cvref_t<decltype(node->next)>>;
};

template <typename T>
concept double_linkable = requires(T *node) {
    requires detail::base_double_linkable<
        std::remove_cvref_t<decltype(node->next)>>;
    requires detail::base_double_linkable<
        std::remove_cvref_t<decltype(node->prev)>>;
};

#define STDX_SINGLE_LINKABLE single_linkable
#define STDX_DOUBLE_LINKABLE double_linkable
#else
#define STDX_SINGLE_LINKABLE typename
#define STDX_DOUBLE_LINKABLE typename
#endif

namespace detail::detect {
template <typename T, typename = void> constexpr auto has_prev_pointer = false;
template <typename T>
constexpr auto
    has_prev_pointer<T, std::void_t<decltype(std::declval<T>().prev)>> = true;
} // namespace detail::detect

namespace node_policy {
template <typename Node> class checked {
    constexpr static auto valid_for_push(Node *node) -> bool {
        if constexpr (detail::detect::has_prev_pointer<Node>) {
            return node->prev == nullptr and node->next == nullptr;
        } else {
            return node->next == nullptr;
        }
    }

  public:
    template <typename L>
    constexpr static auto push_front(L &list, Node *node) -> void {
        if (not valid_for_push(node)) {
            STDX_PANIC("bad list node!");
        }
        list.unchecked_push_front(node);
    }

    template <typename L>
    constexpr static auto push_back(L &list, Node *node) -> void {
        if (not valid_for_push(node)) {
            STDX_PANIC("bad list node!");
        }
        list.unchecked_push_back(node);
    }

    template <typename L, typename It>
    constexpr static auto insert(L &list, It it, Node *node) -> void {
        if (not valid_for_push(node)) {
            STDX_PANIC("bad list node!");
        }
        list.unchecked_insert(it, node);
    }

    constexpr static auto on_pop(Node *node) {
        if constexpr (detail::detect::has_prev_pointer<Node>) {
            node->prev = nullptr;
        }
        node->next = nullptr;
    }

    constexpr static auto on_clear(Node *head) {
        while (head != nullptr) {
            if constexpr (detail::detect::has_prev_pointer<Node>) {
                head->prev = nullptr;
            }
            head = std::exchange(head->next, nullptr);
        }
    }
};

template <typename Node> struct unchecked {
    template <typename L>
    constexpr static auto push_front(L &list, Node *node) -> void {
        list.unchecked_push_front(node);
    }
    template <typename L>
    constexpr static auto push_back(L &list, Node *node) -> void {
        list.unchecked_push_back(node);
    }
    template <typename L, typename It>
    constexpr static auto insert(L &list, It it, Node *node) -> void {
        list.unchecked_insert(it, node);
    }
    constexpr static auto on_pop(Node *) {}
    constexpr static auto on_clear(Node *) {}
};
} // namespace node_policy
} // namespace v1
} // namespace stdx
