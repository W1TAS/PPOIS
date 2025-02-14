#include <iostream>
#include "Graph/Graph.hpp"
#include "Graph/Exceptions.hpp"

int main() {
    try {
        // Создаем граф с вершинами типа int
        Graph<int> graph;

        // Добавляем вершины
        graph.add_vertex(1);
        graph.add_vertex(2);
        graph.add_vertex(3);
        graph.add_vertex(4);
        graph.add_vertex(5);

        // Добавляем рёбра
        graph.add_edge(1, 2);
        graph.add_edge(1, 3);
        graph.add_edge(2, 4);
        graph.add_edge(3, 5);
        graph.add_edge(4, 5);

        std::cout << graph <<"\n";

        // Вывод всех вершин (прямой итератор)
        std::cout << "Vertices (direct): ";
        for (auto it = graph.vertices_begin(); it != graph.vertices_end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Вывод всех вершин (обратный итератор)
        std::cout << "Vertices (reverse): ";
        for (auto it = graph.vertices_rbegin(); it != graph.vertices_rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Вывод смежных вершин для 1 (прямой итератор)
        std::cout << "Adjacent vertices of 1 (direct): ";
        for (auto it = graph.adjacent_vertices_begin(1); it != graph.adjacent_vertices_end(1); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Вывод смежных вершин для 1 (обратный итератор)
        std::cout << "Adjacent vertices of 1 (reverse): ";
        for (auto it = graph.adjacent_vertices_rbegin(1); it != graph.adjacent_vertices_rend(1); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Удаление рёбер
        std::cout << "Removing edge (1 -> 2)...\n";
        auto vertex_it = graph.find_vertex(1); // Используем поиск для получения VertexIterator
        auto adj_it = graph.adjacent_vertices_begin(1); // Получаем AdjacentVertexIterator
        graph.remove_edge(vertex_it, adj_it);

        // Проверка рёбер после удаления
        std::cout << "Adjacent vertices of 1 after removing edge (direct): ";
        for (auto it = graph.adjacent_vertices_begin(1); it != graph.adjacent_vertices_end(1); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Удаление вершины
        std::cout << "Removing vertex 3...\n";
        auto vertex_to_remove = graph.find_vertex(3); // Получаем VertexIterator на вершину 3
        graph.remove_vertex(vertex_to_remove);

        // Вывод всех вершин после удаления вершины 3
        std::cout << "Vertices after removing vertex 3 (direct): ";
        for (auto it = graph.vertices_begin(); it != graph.vertices_end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Вывод смежных вершин для 1 после удаления вершины 3
        std::cout << "Adjacent vertices of 1 after removing vertex 3 (direct): ";
        for (auto it = graph.adjacent_vertices_begin(1); it != graph.adjacent_vertices_end(1); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Проверка рёбер для других вершин
        std::cout << "Adjacent vertices of 4 (direct): ";
        for (auto it = graph.adjacent_vertices_begin(4); it != graph.adjacent_vertices_end(4); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        // Проверка обратных итераторов после всех изменений
        std::cout << "Vertices (reverse) after all changes: ";
        for (auto it = graph.vertices_rbegin(); it != graph.vertices_rend(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";

        std::cout << graph << "\n" ;

        Graph<int>(graph2) = graph;

        std::cout << graph2 << "\n" ;

    } catch (const GraphException& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Unexpected error: " << ex.what() << "\n";
    }

    return 0;
}
