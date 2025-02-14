#ifndef ADJACENTVERTEXITERATORS_TPP
#define ADJACENTVERTEXITERATORS_TPP

#include <algorithm>
#include "Exceptions.hpp"

template <typename T>
class Graph<T>::AdjacentVertexIterator {
public:
    using iterator_category = std::forward_iterator_tag;  // Однопроходной итератор
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;

    // Конструктор итератора, поддерживающий как iterator, так и const_iterator
    explicit AdjacentVertexIterator(typename std::list<VertexNode*>::const_iterator it)
        : current(it) {}

    // Доступ к элементу
    reference operator*() {
        return (*current)->value;
    }

    const_reference operator*() const {
        return (*current)->value;
    }


    // Инкремент
    AdjacentVertexIterator& operator++() {
        ++current;
        return *this;
    }

    AdjacentVertexIterator operator++(int) {
        AdjacentVertexIterator temp = *this;
        ++(*this);
        return temp;
    }

    // Сравнение итераторов
    bool operator==(const AdjacentVertexIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const AdjacentVertexIterator& other) const {
        return !(*this == other);
    }

private:
    typename std::list<VertexNode*>::const_iterator current;  // Итератор для списка рёбер
};

template <typename T>
auto Graph<T>::adjacent_vertices_begin(const_reference value) const {
    // Найти вершину по значению
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
        [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(value));
    }

    // Вернуть итератор на первый элемент в списке рёбер
    return AdjacentVertexIterator(vertex_it->edges.begin());
}

template <typename T>
auto Graph<T>::adjacent_vertices_end(const_reference value) const {
    // Найти вершину по значению
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
        [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(value));
    }

    // Вернуть итератор на конец списка рёбер
    return AdjacentVertexIterator(vertex_it->edges.end());
}

// Reverse итератор для смежных вершин
template <typename T>
class Graph<T>::AdjacentVertexReverseIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using iterator = typename std::list<VertexNode*>::reverse_iterator;

    // Конструкторы
    explicit AdjacentVertexReverseIterator(typename std::list<VertexNode*>::const_reverse_iterator it)
            : current(it) {}

    // Доступ к элементу
    reference operator*() {
        return (*current)->value;
    }

    const_reference operator*() const {
        return (*current)->value;
    }

    // Инкремент
    AdjacentVertexReverseIterator& operator++() {
        ++current;
        return *this;
    }

    AdjacentVertexReverseIterator operator++(int) {
        AdjacentVertexReverseIterator temp = *this;
        ++(*this);
        return temp;
    }

    AdjacentVertexReverseIterator& operator--() {
        --current;
        return *this;
    }

    AdjacentVertexReverseIterator operator--(int) {
        AdjacentVertexReverseIterator temp = *this;
        --(*this);
        return temp;
    }

    // Сравнение
    bool operator==(const AdjacentVertexReverseIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const AdjacentVertexReverseIterator& other) const {
        return !(*this == other);
    }

private:
    typename std::list<VertexNode*>::const_reverse_iterator current;
};


// Методы доступа в Graph.tpp
template <typename T>
auto Graph<T>::adjacent_vertices_rbegin(const_reference value) const {
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
                                  [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found.");
    }

    return AdjacentVertexReverseIterator(vertex_it->edges.rbegin());
}

template <typename T>
auto Graph<T>::adjacent_vertices_rend(const_reference value) const {
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
                                  [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found.");
    }

    return AdjacentVertexReverseIterator(vertex_it->edges.rend());
}


#endif //ADJACENTVERTEXITERATORS_TPP
