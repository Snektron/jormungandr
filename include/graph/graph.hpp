#ifndef _JORMUNGANDR_GRAPH_GRAPH_HPP
#define _JORMUNGANDR_GRAPH_GRAPH_HPP

#include <vector>
#include <concepts>
#include <span>

template <typename F, typename T>
concept ForEachNodeCallback = std::invocable<F, T, std::span<const T>>;

template <std::unsigned_integral T>
class Graph {
    public:
        struct Node {
            size_t first_edge;
            size_t num_edges;
        };

    private:
        std::vector<T> edges;
        std::vector<Node> nodes;
    public:
        Graph() = default;
        ~Graph() = default;

        auto for_each(ForEachNodeCallback<T> auto f) const -> void;
};

template <std::unsigned_integral T>
auto Graph<T>::for_each(ForEachNodeCallback<T> auto f) const -> void {
    for (T i = 0; i < this->nodes.size(); ++i) {
        auto [start, len] = this->nodes[i];
        auto span = std::span(&this->edges[i], len);
        f(i, span);
    }
}

#endif
