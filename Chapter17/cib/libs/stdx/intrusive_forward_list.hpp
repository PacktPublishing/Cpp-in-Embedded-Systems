#pragma once

#include <stdx/detail/list_common.hpp>

#include <cstddef>
#if __cplusplus < 202002L
#include <iterator>
#endif

namespace stdx {
inline namespace v1 {
template <STDX_SINGLE_LINKABLE NodeType,
          template <typename> typename P = node_policy::checked>
class intrusive_forward_list {
    friend P<NodeType>;

    template <typename N> struct iterator_t {
        using difference_type = std::ptrdiff_t;
        using value_type = N;
        using pointer = value_type *;
        using reference = value_type &;
#if __cplusplus < 202002L
        using iterator_category = std::forward_iterator_tag;
#endif

        constexpr iterator_t() = default;
        constexpr explicit iterator_t(pointer n) : node{n} {}

        constexpr auto operator*() -> reference { return *node; }
        constexpr auto operator*() const -> reference { return *node; }
        constexpr auto operator->() -> pointer { return node; }
        constexpr auto operator->() const -> pointer { return node; }

        constexpr auto operator++() -> iterator_t & {
            node = node->next;
            return *this;
        }
        constexpr auto operator++(int) -> iterator_t {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

      private:
        pointer node{};

#if __cpp_impl_three_way_comparison < 201907L
        friend constexpr auto operator==(iterator_t lhs,
                                         iterator_t rhs) -> bool {
            return lhs.node == rhs.node;
        }
        friend constexpr auto operator!=(iterator_t lhs,
                                         iterator_t rhs) -> bool {
            return not(lhs == rhs);
        }
#else
        friend constexpr auto operator==(iterator_t,
                                         iterator_t) -> bool = default;
#endif
    };

  public:
    using value_type = NodeType;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = value_type const &;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using iterator = iterator_t<value_type>;
    using const_iterator = iterator_t<value_type const>;

  private:
    pointer head{};
    pointer tail{};

    constexpr auto unchecked_push_front(pointer n) -> void {
        n->next = head;
        head = n;
        if (tail == nullptr) {
            tail = n;
        }
    }

    constexpr auto unchecked_push_back(pointer n) -> void {
        if (tail != nullptr) {
            tail->next = n;
        }
        tail = n;
        n->next = nullptr;
        if (head == nullptr) {
            head = n;
        }
    }

  public:
    constexpr auto begin() -> iterator { return iterator{head}; }
    constexpr auto begin() const -> const_iterator {
        return const_iterator{head};
    }
    constexpr auto cbegin() const -> const_iterator {
        return const_iterator{head};
    }
    constexpr auto end() -> iterator { return {}; }
    constexpr auto end() const -> const_iterator { return {}; }
    constexpr auto cend() const -> const_iterator { return {}; }

    constexpr auto front() const -> reference { return *head; }
    constexpr auto back() const -> reference { return *tail; }

    constexpr auto push_front(pointer n) -> void {
        P<NodeType>::push_front(*this, n);
    }
    constexpr auto push_back(pointer n) -> void {
        P<NodeType>::push_back(*this, n);
    }

    constexpr auto pop_front() -> pointer {
        pointer poppedNode = head;
        head = head->next;

        if (head == nullptr) {
            tail = nullptr;
        }

        P<NodeType>::on_pop(poppedNode);
        return poppedNode;
    }

    [[nodiscard]] constexpr auto empty() const -> bool {
        return head == nullptr;
    }

    constexpr auto clear() -> void {
        P<NodeType>::on_clear(head);
        head = nullptr;
        tail = nullptr;
    }
};
} // namespace v1
} // namespace stdx
