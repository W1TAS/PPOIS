#ifndef GRAPH_TPP
#define GRAPH_TPP

#include "Graph.hpp"
#include "Exceptions.hpp"
#include <algorithm>

// Конструктор копирования
template <typename T>
Graph<T>::Graph(const Graph& other) {
    for (const auto& vertex : other.vertices) {
        add_vertex(vertex.value);
    }
    for (const auto& vertex : other.vertices) {
        for (const auto& edge : vertex.edges) {
            add_edge(vertex.value, edge->value);
        }
    }
}

// Деструктор
template <typename T>
Graph<T>::~Graph() {
    clear();
}

// Оператор присваивания
template <typename T>
Graph<T>& Graph<T>::operator=(const Graph& other) {
    if (this != &other) {
        clear();
        for (const auto& vertex : other.vertices) {
            add_vertex(vertex.value);
        }
        for (const auto& vertex : other.vertices) {
            for (const auto& edge : vertex.edges) {
                add_edge(vertex.value, edge->value);
            }
        }
    }
    return *this;
}

// Проверка на пустоту
template <typename T>
bool Graph<T>::empty() const noexcept {
    return vertices.empty();
}

// Очистка графа
template <typename T>
void Graph<T>::clear() noexcept {
    vertices.clear();
}

// Добавление вершины
template <typename T>
void Graph<T>::add_vertex(const_reference value) {
    if (has_vertex(value)) {
        throw GraphException("Vertex already exists: " + std::to_string(value));
    }
    vertices.emplace_back(value);
}

// Добавление ребра
template <typename T>
void Graph<T>::add_edge(const_reference from, const_reference to) {
    auto from_vertex = std::find_if(vertices.begin(), vertices.end(),
        [&from](const VertexNode& v) { return v.value == from; });

    auto to_vertex = std::find_if(vertices.begin(), vertices.end(),
        [&to](const VertexNode& v) { return v.value == to; });

    if (from_vertex == vertices.end() || to_vertex == vertices.end()) {
        throw GraphException("One or both vertices not found");
    }

    from_vertex->edges.push_back(&(*to_vertex));
}

// Удаление вершины
template <typename T>
void Graph<T>::remove_vertex(const_reference value) {
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
        [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(value));
    }

    // Удалить все рёбра, инцидентные удаляемой вершине
    for (auto& vertex : vertices) {
        vertex.edges.remove(&(*vertex_it));
    }

    vertices.erase(vertex_it);
}

// Удаление ребра
template <typename T>
void Graph<T>::remove_edge(const_reference from, const_reference to) {
    auto from_vertex = std::find_if(vertices.begin(), vertices.end(),
        [&from](const VertexNode& v) { return v.value == from; });

    if (from_vertex == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(from));
    }

    auto to_vertex = std::find_if(vertices.begin(), vertices.end(),
        [&to](const VertexNode& v) { return v.value == to; });

    if (to_vertex == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(to));
    }

    from_vertex->edges.remove(&(*to_vertex));
}

template <typename T>
void Graph<T>::remove_vertex(VertexIterator pos) {
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
                                  [&pos](const VertexNode& v) { return v.value == *pos; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found for deletion.");
    }

    // Удаляем все ребра, инцидентные этой вершине
    for (auto& vertex : vertices) {
        vertex.edges.remove(&(*vertex_it));
    }

    vertices.erase(vertex_it);
}

template <typename T>
void Graph<T>::remove_edge(VertexIterator from, AdjacentVertexIterator to) {
    auto from_vertex_it = std::find_if(vertices.begin(), vertices.end(),
                                       [&from](const VertexNode& v) { return v.value == *from; });

    if (from_vertex_it == vertices.end()) {
        throw GraphException("Source vertex not found.");
    }

    auto to_vertex = std::find_if(from_vertex_it->edges.begin(), from_vertex_it->edges.end(),
                                  [&to](VertexNode* neighbor) { return neighbor->value == *to; });

    if (to_vertex == from_vertex_it->edges.end()) {
        throw GraphException("Edge not found.");
    }

    from_vertex_it->edges.erase(to_vertex);
}


template<typename T>
bool Graph<T>::has_vertex(const_reference value) const {
    return std::any_of(vertices.cbegin(), vertices.cend(),
                       [&value](const VertexNode& vertex) { return vertex.value == value; });
}

template <typename T>
bool Graph<T>::has_edge(const_reference from, const_reference to) const {
    auto from_vertex = std::find_if(vertices.begin(), vertices.end(),
                                    [&from](const VertexNode& vertex) { return vertex.value == from; });

    if (from_vertex == vertices.end()) return false;

    return std::any_of(from_vertex->edges.begin(), from_vertex->edges.end(),
                       [&to](const VertexNode* edge) { return edge->value == to; });
}


// Количество вершин
template <typename T>
size_t Graph<T>::vertex_count() const noexcept {
    return vertices.size();
}

// Количество рёбер
template <typename T>
size_t Graph<T>::edge_count() const noexcept {
    size_t count = 0;
    for (const auto& vertex : vertices) {
        count += vertex.edges.size();
    }
    return count;
}

// Степень вершины
template <typename T>
size_t Graph<T>::degree(const_reference value) const {
    auto vertex_it = std::find_if(vertices.begin(), vertices.end(),
        [&value](const VertexNode& v) { return v.value == value; });

    if (vertex_it == vertices.end()) {
        throw GraphException("Vertex not found: " + std::to_string(value));
    }

    return vertex_it->edges.size();
}


template<typename T>
bool Graph<T>::operator==(const Graph& other) const {
    // Сначала проверяем, что количество вершин одинаково
    if (vertices.size() != other.vertices.size()) {
        return false;
    }

    // Для каждой вершины проверяем соответствие значений и списка смежных вершин
    auto it1 = vertices.begin();
    auto it2 = other.vertices.begin();
    for (; it1 != vertices.end(); ++it1, ++it2) {
        // Сравниваем значение вершины
        if (it1->value != it2->value) {
            return false;
        }

        // Сравниваем списки смежных вершин
        if (it1->edges.size() != it2->edges.size()) {
            return false;
        }

        // Сортируем и сравниваем смежные вершины по значениям
        std::list<T> adj1_values, adj2_values;
        for (auto& adj : it1->edges) {
            adj1_values.push_back(adj->value); // Добавляем значения смежных вершин
        }
        for (auto& adj : it2->edges) {
            adj2_values.push_back(adj->value); // Добавляем значения смежных вершин
        }

        // Сортируем значения смежных вершин для каждой вершины
        adj1_values.sort();
        adj2_values.sort();

        // Сравниваем отсортированные списки смежных вершин
        if (adj1_values != adj2_values) {
            return false;
        }
    }

    return true;
}


template<typename T>
bool Graph<T>::operator!=(const Graph& other) const {
    return !(*this == other);
}

template<typename T>
bool Graph<T>::operator<(const Graph& other) const {
    if (this->vertex_count() == other.vertex_count())
    {
        return this->edge_count() < other.edge_count();
    } else
    {
        return this->vertex_count()<other.vertex_count();
    }
}

template<typename T>
bool Graph<T>::operator>(const Graph& other) const {
    if (this->vertex_count() == other.vertex_count())
    {
        return this->edge_count() > other.edge_count();
    } else {
        return this->vertex_count()>other.vertex_count();
    }
}

template<typename T>
bool Graph<T>::operator<=(const Graph& other) const {
    return (this->vertex_count() == other.vertex_count() and this->edge_count() == other.edge_count()) or (*this < other);
}

template<typename T>
bool Graph<T>::operator>=(const Graph& other) const {
    return (this->vertex_count() == other.vertex_count() and this->edge_count() == other.edge_count()) or (*this > other);
}


template<typename T>
typename Graph<T>::VertexIterator Graph<T>::find_vertex(const_reference value) {
    // Ищем вершину в списке вершин
    auto it = std::find_if(vertices.begin(), vertices.end(), [&value](const VertexNode& node) {
        return node.value == value;
    });

    // Если вершина не найдена, выбрасываем исключение
    if (it == vertices.end()) {
        throw GraphException("Vertex not found.");
    }

    // Возвращаем VertexIterator
    return VertexIterator(it);
}


template <typename T>
std::ostream& operator<<(std::ostream& os, const Graph<T>& graph) {
    // Обрабатываем каждую вершину
    std::for_each(graph.vertices.begin(), graph.vertices.end(),
        [&os](const typename Graph<T>::VertexNode& vertex) {
            os << vertex.value << ": ";
            // Обрабатываем смежные вершины для каждой вершины
            std::for_each(vertex.edges.begin(), vertex.edges.end(),
                [&os](const typename Graph<T>::VertexNode* edge) {
                    os << edge->value << " ";
                });
            os << "\n";
        });
    return os;
}

#endif // GRAPH_TPP
