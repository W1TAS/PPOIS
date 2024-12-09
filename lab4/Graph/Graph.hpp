#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <list>


template <typename T>
class Graph {
public:
    // Конструкторы и деструктор
    Graph() = default;
    Graph(const Graph& other);
    ~Graph();

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    // Итераторы (объявления, реализация в VertexIterators.tpp)
    class VertexIterator;
    class AdjacentVertexIterator;
    class VertexReverseIterator;
    class AdjacentVertexReverseIterator;

    auto vertices_begin() const { return vertices.cbegin(); }
    auto vertices_end() const { return vertices.cend(); }

    auto adjacent_vertices_begin(const_reference value) const;
    auto adjacent_vertices_end(const_reference value) const;

    auto vertices_rbegin() const { return VertexReverseIterator(vertices.rbegin()); }
    auto vertices_rend() const { return VertexReverseIterator(vertices.rend()); }

    auto adjacent_vertices_rbegin(const_reference value) const;
    auto adjacent_vertices_rend(const_reference value) const;



    Graph& operator=(const Graph& other);

    // Основные методы
    bool empty() const noexcept;
    void clear() noexcept;

    void add_vertex(const_reference value);
    void add_edge(const_reference from, const_reference to);

    void remove_vertex(const_reference value);
    void remove_edge(const_reference from, const_reference to);

    void remove_vertex(VertexIterator pos);
    void remove_edge(VertexIterator from, AdjacentVertexIterator to);


    bool has_vertex(const_reference value) const;
    bool has_edge(const_reference from, const_reference to) const;

    size_t vertex_count() const noexcept;
    size_t edge_count() const noexcept;

    size_t degree(const_reference value) const;



    bool operator==(const Graph&) const;
    bool operator!=(const Graph&) const;
    bool operator<(const Graph&) const;
    bool operator<=(const Graph&) const;
    bool operator>(const Graph&) const;
    bool operator>=(const Graph&) const;

    VertexIterator find_vertex(const_reference value);


    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Graph<U>& graph);

private:
    struct VertexNode {
        T value;
        std::list<VertexNode*> edges;
        bool operator==(const VertexNode& other) const {
            return value == other.value;
        }
        VertexNode(const_reference val) : value(val) {}
        friend std::ostream& operator<<(std::ostream& os, const VertexNode& node) {
            os << node.value; // Вывод значения вершины
            return os;
        }

    };

    std::list<VertexNode> vertices;
};

#include "VertexIterators.tpp"
#include "AdjacentVertexIterators.tpp"
#include "Graph.tpp"

#endif // GRAPH_HPP
