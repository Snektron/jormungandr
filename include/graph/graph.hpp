#ifndef _JORMUNGANDR_GRAPH_GRAPH_HPP
#define _JORMUNGANDR_GRAPH_GRAPH_HPP

#include <vector>

template <typename T>
struct Edge {
    T from;
    T to;

    Edge(T, T);

    auto operator<=>(const Edge&) const -> std::strong_ordering = default;
};

template <typename T>
class Graph {
    private:
        std::vector<Edge<T>> edges;
    public:
        Graph() = default;
        ~Graph() = default;

        auto add_edge(T, T) -> void;
};

template <typename T>
Edge<T>::Edge(T from, T to) : from(from), to(to) {}

template <typename T>
auto Graph<T>::add_edge(T from, T to) -> void {
    this->edges.emplace_back(from, to);
}

#endif
