#ifndef _JORMUNGANDR_GRAPH_GRAPH_HPP
#define _JORMUNGANDR_GRAPH_GRAPH_HPP

#include <vector>
#include <concepts>
#include <span>
#include <numeric>
#include <cassert>

template <typename F, typename T>
concept ForEachNeighbourCallback = std::invocable<F, T>;

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
        Graph(std::vector<T>&& srcs, std::vector<T>&& dsts);
        ~Graph() = default;

        auto for_each_neighbour(T node, ForEachNeighbourCallback<T> auto f) const -> void;
        auto for_each(ForEachNodeCallback<T> auto f) const -> void;
        auto num_nodes() const -> size_t;
};

template <std::unsigned_integral T>
Graph<T>::Graph(std::vector<T>&& srcs, std::vector<T>&& dsts) {
    assert(srcs.size() == dsts.size());
    if (srcs.size() == 0)
        return;

    size_t total_nodes = 0;
    for (size_t i = 0; i < srcs.size(); ++i) {
        total_nodes = srcs[i] > total_nodes ? srcs[i] : total_nodes;
        total_nodes = dsts[i] > total_nodes ? dsts[i] : total_nodes;
    }
    ++total_nodes;

    this->nodes.resize(total_nodes, {0, 0});
    for (auto& src : srcs) {
        ++this->nodes[src].num_edges;
    }

    size_t offset = 0;
    for (auto& node : this->nodes) {
        node.first_edge = offset;
        offset += node.num_edges;
    }

    auto indices = std::vector<size_t>(srcs.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
        return srcs[i] < srcs[j];
    });

    // Re-use edge list
    this->edges = std::move(srcs);
    for (size_t i = 0; i < indices.size(); ++i) {
        this->edges[i] = dsts[indices[i]];
    }

    for (T i = 0; i < this->nodes.size(); ++i) {
        auto [start, len] = this->nodes[i];
        auto span = std::span(&this->edges[start], len);
        std::sort(span.begin(), span.end());
    }
}

template <std::unsigned_integral T>
auto Graph<T>::for_each_neighbour(T node, ForEachNeighbourCallback<T> auto f) const -> void {
    auto [start, len] = this->nodes[node];
    for (size_t i = 0; i < len; ++i) {
        f(this->edges[i + start]);
    }
}

template <std::unsigned_integral T>
auto Graph<T>::for_each(ForEachNodeCallback<T> auto f) const -> void {
    for (T i = 0; i < this->nodes.size(); ++i) {
        auto [start, len] = this->nodes[i];
        auto span = std::span(&this->edges[start], len);
        f(i, span);
    }
}

template <std::unsigned_integral T>
auto Graph<T>::num_nodes() const -> size_t {
    return this->nodes.size();
}

#endif
