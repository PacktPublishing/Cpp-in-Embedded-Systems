#include <flow/detail/walk.hpp>

#include <stdx/tuple_algorithms.hpp>

#include <set>
#include <string>
#include <string_view>

namespace flow {
using VizFunctionPtr = auto (*)() -> std::string;

struct graphviz_builder {
    template <typename Graph>
    [[nodiscard]] static auto build(Graph const &input) {
        auto const nodes = flow::dsl::get_nodes(input);
        auto const edges = flow::dsl::get_edges(input);

        using nodes_t = std::set<std::string_view>;
        nodes_t sources{};
        nodes_t sinks{};
        for_each(
            [&]<typename Node>(Node const &) {
                sources.insert(Node::name_t::value);
                sinks.insert(Node::name_t::value);
            },
            nodes);
        for_each(
            [&]<typename Edge>(Edge const &) {
                sinks.erase(Edge::source_t::name_t::value);
                sources.erase(Edge::dest_t::name_t::value);
            },
            edges);

        std::string output{"digraph "};
        output += std::string_view{Graph::name};
        output += " {\n";
        for (auto const &node : sources) {
            output += "start -> " + std::string{node} + '\n';
        }
        for_each(
            [&]<typename Edge>(Edge const &) {
                output += std::string{Edge::source_t::name_t::value};
                output += " -> ";
                output += std::string{Edge::dest_t::name_t::value};
                output += '\n';
            },
            edges);
        for (auto const &node : sinks) {
            output += std::string{node} + " -> end\n";
        }
        output += "}";
        return output;
    }

    template <typename Initialized> class built_flow {
        static auto built() {
            auto const v = Initialized::value;
            return build(v);
        }

        static auto run() -> std::string { return built(); }

      public:
        // NOLINTNEXTLINE(google-explicit-constructor)
        constexpr explicit(false) operator VizFunctionPtr() const {
            return run;
        }
        auto operator()() const -> std::string { return run(); }
        constexpr static bool active = true;
    };

    template <typename Initialized>
    [[nodiscard]] constexpr static auto render() -> built_flow<Initialized> {
        return {};
    }
};
} // namespace flow
