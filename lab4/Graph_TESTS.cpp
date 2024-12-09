#include <gtest/gtest.h>
#include "../Graph/Graph.hpp"
#include "../Graph/Exceptions.hpp"

// Тест конструктора и деструктора
TEST(GraphTest, ConstructorDestructor) {
    // Проверяем, что граф создается и уничтожается без ошибок
    Graph<int> graph;
    EXPECT_TRUE(graph.empty());  // После создания граф должен быть пустым

    // Добавим вершины и рёбра
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    EXPECT_FALSE(graph.empty());  // Граф больше не пустой
    EXPECT_EQ(graph.vertex_count(), 2);
    EXPECT_EQ(graph.edge_count(), 1);

    // Проверим удаление
    graph.remove_edge(1, 2);
    EXPECT_EQ(graph.edge_count(), 0);
    graph.remove_vertex(1);
    graph.remove_vertex(2);
    EXPECT_TRUE(graph.empty());
}

TEST(GraphTest, AddRemoveVertexEdge) {
    Graph<int> graph;

    // Проверка добавления вершин
    graph.add_vertex(1);
    graph.add_vertex(2);
    EXPECT_EQ(graph.vertex_count(), 2);

    // Проверка добавления рёбер
    graph.add_edge(1, 2);
    EXPECT_EQ(graph.edge_count(), 1);

    // Проверка удаления рёбер
    graph.remove_edge(1, 2);
    EXPECT_EQ(graph.edge_count(), 0);

    // Проверка удаления вершин
    graph.remove_vertex(1);
    graph.remove_vertex(2);
    EXPECT_TRUE(graph.empty());
}

TEST(GraphTest, FindVertexEdge) {
    Graph<int> graph;

    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Тестирование поиска вершин
    auto it = graph.find_vertex(1);
    EXPECT_EQ(*it, 1);

    // Тестирование поиска рёбер
    EXPECT_TRUE(graph.has_edge(1, 2));
    EXPECT_FALSE(graph.has_edge(2, 1));  // Направленное ребро
}

TEST(GraphTest, ComparisonOperators) {
    Graph<int> graph1, graph2;
    graph1.add_vertex(1);
    graph1.add_vertex(2);
    graph1.add_edge(1, 2);

    graph2.add_vertex(1);
    graph2.add_vertex(2);
    graph2.add_edge(1, 2);

    // Проверка на равенство
    EXPECT_TRUE(graph1 == graph2);
    EXPECT_FALSE(graph1 != graph2);

    // Изменим графы и проверим неравенство
    graph2.remove_edge(1, 2);
    EXPECT_FALSE(graph1 == graph2);
    EXPECT_TRUE(graph1 != graph2);

    // Проверка операторов <, >, <=, >=
    EXPECT_TRUE(graph1 > graph2);
    EXPECT_TRUE(graph2 < graph1);
    EXPECT_TRUE(graph1 >= graph2);
    EXPECT_TRUE(graph2 <= graph1);
}

TEST(GraphTest, Iterators) {
    Graph<int> graph;

    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);

    // Проверка обычных итераторов
    auto it = graph.vertices_begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);

    // Проверка реверсивных итераторов
    auto rit = graph.vertices_rbegin();
    EXPECT_EQ(*rit, 3);
    ++rit;
    EXPECT_EQ(*rit, 2);
    ++rit;
    EXPECT_EQ(*rit, 1);
}

TEST(GraphTest, AdjacentVertexIterators) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);
    graph.add_edge(2, 1);

    // Проверка итераторов для смежных вершин
    auto adj1 = graph.adjacent_vertices_begin(1);
    EXPECT_EQ(*adj1, 2);
    ++adj1;
    EXPECT_EQ(adj1, graph.adjacent_vertices_end(1));  // Должно быть конец

    auto adj2 = graph.adjacent_vertices_begin(2);
    EXPECT_EQ(*adj2, 3);
    ++adj2;
    EXPECT_EQ(*adj2, 1);
}

TEST(GraphTest, Exceptions) {
    Graph<int> graph;

    // Проверка на исключение при добавлении одинаковых вершин
    graph.add_vertex(1);
    EXPECT_THROW(graph.add_vertex(1), GraphException);

    // Проверка на исключение при добавлении рёбер в несуществующие вершины
    EXPECT_THROW(graph.add_edge(1, 2), GraphException);

    // Проверка на исключение при удалении несуществующих вершин и рёбер
    EXPECT_THROW(graph.remove_vertex(2), GraphException);
    EXPECT_THROW(graph.remove_edge(1, 2), GraphException);
    EXPECT_THROW(graph.remove_edge(2, 1), GraphException);
}

TEST(GraphTest, EmptyGraph) {
    Graph<int> graph;
    EXPECT_TRUE(graph.empty());

    graph.add_vertex(1);
    EXPECT_FALSE(graph.empty());

    graph.remove_vertex(1);
    EXPECT_TRUE(graph.empty());
}

TEST(GraphTest, VertexDegree) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    graph.add_edge(1, 2);
    graph.add_edge(2, 3);

    EXPECT_EQ(graph.degree(1), 1);
    EXPECT_EQ(graph.degree(2), 1);
    EXPECT_EQ(graph.degree(3), 0);
}

TEST(GraphTest, ClearGraph) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    graph.clear();
    EXPECT_TRUE(graph.empty());
}

TEST(GraphTest, TestCopyConstructor) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Копирование графа
    Graph<int> graph_copy = graph;

    // Проверяем, что в графе скопированы вершины
    EXPECT_EQ(graph_copy, graph);
}



TEST(GraphTest, TestCopyAssignmentOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    Graph<int> graph_copy;
    graph_copy = graph;

    // Проверяем, что в графе скопированы вершины
    EXPECT_EQ(graph_copy.vertex_count(), graph.vertex_count());
    EXPECT_EQ(graph_copy.edge_count(), graph.edge_count());
}

TEST(GraphTest, TestRemoveEdgeVertexNotFound) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Попытка удалить рёбро для несуществующей вершины
    EXPECT_THROW(graph.remove_edge(100, 2), GraphException);
}

TEST(GraphTest, TestRemoveVertexVertexNotFound) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);

    // Попытка удалить несуществующую вершину
    EXPECT_THROW(graph.remove_vertex(100), GraphException);
}

TEST(GraphTest, TestRemoveVertex) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);

    // Удаление вершины
    auto initial_vertex_count = graph.vertex_count();
    auto vertex_to_remove = graph.find_vertex(3);
    graph.remove_vertex(vertex_to_remove);

    EXPECT_EQ(graph.vertex_count(), initial_vertex_count - 1);
    EXPECT_THROW(graph.find_vertex(3), GraphException);
}

TEST(GraphTest, TestDegreeVertexNotFound) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);

    // Проверка метода degree для несуществующей вершины
    EXPECT_THROW(graph.degree(100), GraphException);
}

TEST(GraphTest, TestEqualityOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    Graph<int> graph_copy = graph;
    EXPECT_TRUE(graph == graph_copy);
}

TEST(GraphTest, TestInequalityOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    Graph<int> graph_copy;
    graph_copy.add_vertex(3);
    EXPECT_TRUE(graph != graph_copy);
}

TEST(GraphTest, TestFindVertexNotFound) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);

    // Проверка на поиск несуществующей вершины
    EXPECT_THROW(graph.find_vertex(100), GraphException);
}

TEST(GraphTest, TestOutputStreamOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_edge(1, 2);
    graph.add_edge(2, 3);

    std::ostringstream oss;
    oss << graph;
    std::string expected_output = "1: 2 \n2: 3 \n3: \n";
    EXPECT_EQ(oss.str(), expected_output);

    std::ostringstream oss2;
    // Вывод всех вершин (прямой итератор)
    for (auto it = graph.vertices_begin(); it != graph.vertices_end(); ++it) {
        oss2 << *it << " ";
    }
    std::string expected_output2 = "1 2 3";
    EXPECT_EQ(oss.str(), expected_output);
}

TEST(GraphTest, TestRemoveEdgeByIterators) {
    Graph<int> graph;

    // Добавляем вершины
    graph.add_vertex(1);
    graph.add_vertex(2);
    // Добавляем рёбра
    graph.add_edge(1, 2);

    Graph<int> graph2;
    graph2.add_vertex(1);
    graph2.add_vertex(2);
    auto vertex_it = graph.find_vertex(1); // Используем поиск для получения VertexIterator
    auto adj_it = graph.adjacent_vertices_begin(1); // Получаем AdjacentVertexIterator
    graph.remove_edge(vertex_it, adj_it);
    EXPECT_EQ(graph2,graph);

    EXPECT_THROW(graph.remove_edge(vertex_it, adj_it), GraphException);
}

TEST(GraphTest, TestAdjacentVertexIterator) {
    Graph<int> graph;

    // Добавляем вершины и рёбра
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Получаем итератор на смежные вершины
    auto adj_it = graph.adjacent_vertices_begin(1);

    // Проверка разыменования итератора
    EXPECT_EQ(*adj_it, 2);  // Вершина 1 имеет соседку 2

    auto adj2 = adj_it++;
    EXPECT_EQ(adj2, graph.adjacent_vertices_begin(1));
    EXPECT_EQ(adj_it, graph.adjacent_vertices_end(1));

    auto another_adj_it = graph.adjacent_vertices_begin(1);
    EXPECT_TRUE(adj_it != another_adj_it);  // Разные итераторы


    adj_it = graph.adjacent_vertices_begin(1);
    // Проверка доступа через константный итератор
    const auto& const_adj_it = adj_it;
    EXPECT_EQ(*const_adj_it, 2);  // Доступ через константный итератор
}

TEST(GraphTest, TestAdjacentVerticesReverseBeginEnd) {
    Graph<int> graph;

    // Добавляем вершины и рёбра
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Проверка метода adjacent_vertices_rbegin
    auto adj_rbegin = graph.adjacent_vertices_rbegin(1);
    EXPECT_EQ(*adj_rbegin, 2);  // Проверяем, что реверсированный итератор находит соседку 2

    // Проверка метода adjacent_vertices_rend
    auto adj_rend = graph.adjacent_vertices_rend(1);
    EXPECT_EQ(++adj_rbegin, adj_rend);  // У нас только одна смежная вершина, реверсированный конец должен быть равен реверсированному началу
}

TEST(GraphTest, TestAdjacentVertexReverseIterator) {
    Graph<int> graph;

    // Добавляем вершины и рёбра
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_edge(1, 2);

    // Получаем обратный итератор на смежные вершины
    auto vertex_it = graph.find_vertex(1);
    auto adj_rbegin = graph.adjacent_vertices_rbegin(1);
    auto adj_rend = graph.adjacent_vertices_rend(1);

    // Проверка разыменования обратного итератора
    EXPECT_EQ(*adj_rbegin, 2);  // Вершина 1 имеет соседку 2, реверс итератор должен начать с 2

    // Проверка инкремента обратного итератора
    ++adj_rbegin;
    EXPECT_EQ(adj_rbegin, adj_rend);  // После инкремента мы должны попасть на конец реверсированного списка

    adj_rbegin = graph.adjacent_vertices_rbegin(1);
    // Проверка пост-инкремента обратного итератора
    auto temp_rbegin = adj_rbegin++;
    EXPECT_EQ(temp_rbegin, graph.adjacent_vertices_rbegin(1));  // До инкремента это была вершина 2
    EXPECT_EQ(adj_rbegin, adj_rend);  // После инкремента мы находимся в конце

    // Проверка сравнения обратных итераторов
    auto another_adj_rbegin = graph.adjacent_vertices_rbegin(1);
    EXPECT_TRUE(adj_rbegin != another_adj_rbegin);  // Разные итераторы

    adj_rbegin = graph.adjacent_vertices_rbegin(1);
    // Проверка доступа через константный реверс-итератор
    const auto& const_adj_rbegin = adj_rbegin;
    EXPECT_EQ(*const_adj_rbegin, 2);  // Доступ через константный реверс-итератор

    // Проверка префиксного декремента
    adj_rend = graph.adjacent_vertices_rend(1);
    EXPECT_EQ(adj_rbegin, --adj_rend);  // После декремента мы вернулись к первой вершине

    adj_rend = graph.adjacent_vertices_rend(1);
    // Проверка пост-инкремента обратного итератора
    auto temp_rend = adj_rend--;
    EXPECT_EQ(temp_rend, graph.adjacent_vertices_rend(1));  // До инкремента это была вершина 2
    EXPECT_EQ(adj_rend, adj_rbegin);  // После инкремента мы находимся в конце
}

TEST(VertexIteratorTest, BeginPointsToFirstVertex) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it = graph.vertices_begin();
    ASSERT_EQ(*it, 1);
}

TEST(VertexIteratorTest, EndPointsToPastLastVertex) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it = graph.vertices_end();
    --it;
    ASSERT_EQ(*it, 3);
}

TEST(VertexIteratorTest, IncrementOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it = graph.vertices_begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(*it, 3);
}

TEST(VertexIteratorTest, DecrementOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it = graph.vertices_end();
    --it;
    ASSERT_EQ(*it, 3);
    --it;
    ASSERT_EQ(*it, 2);
    --it;
    ASSERT_EQ(*it, 1);
}

TEST(VertexIteratorTest, EqualityOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it1 = graph.vertices_begin();
    auto it2 = graph.vertices_begin();
    ASSERT_TRUE(it1 == it2);
    ++it1;
    ASSERT_FALSE(it1 == it2);
}

TEST(VertexIteratorTest, InequalityOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it1 = graph.vertices_begin();
    auto it2 = graph.vertices_end();
    ASSERT_TRUE(it1 != it2);
    ++it1;
    ++it1;
    ++it1;
    ASSERT_FALSE(it1 != it2);
}

TEST(VertexIteratorTest, DereferenceOperator) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto it = graph.vertices_begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
}

TEST(VertexIteratorTest, ReverseIteratorBegin) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt = graph.vertices_rbegin();
    ASSERT_EQ(*rIt, 3);
}

TEST(VertexIteratorTest, ReverseIteratorEnd) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt = graph.vertices_rend();
    --rIt;
    ASSERT_EQ(*rIt, 1);
}

TEST(VertexIteratorTest, ReverseIteratorIncrement) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt = graph.vertices_rbegin();
    ASSERT_EQ(*rIt, 3);
    ++rIt;
    ASSERT_EQ(*rIt, 2);
    ++rIt;
    ASSERT_EQ(*rIt, 1);
}

TEST(VertexIteratorTest, ReverseIteratorDecrement) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt = graph.vertices_rend();
    --rIt;
    ASSERT_EQ(*rIt, 1);
    --rIt;
    ASSERT_EQ(*rIt, 2);
    --rIt;
    ASSERT_EQ(*rIt, 3);
}

TEST(VertexIteratorTest, ReverseIteratorEquality) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt1 = graph.vertices_rbegin();
    auto rIt2 = graph.vertices_rbegin();
    ASSERT_TRUE(rIt1 == rIt2);
    ++rIt1;
    ASSERT_FALSE(rIt1 == rIt2);
}

TEST(VertexIteratorTest, ReverseIteratorInequality) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);

    auto rIt1 = graph.vertices_rbegin();
    auto rIt2 = graph.vertices_rend();
    ASSERT_TRUE(rIt1 != rIt2);
    ++rIt1;
    ++rIt1;
    ++rIt1;
    ASSERT_FALSE(rIt1 != rIt2);
}

// TEST(VertexIteratorTest, CompareWithConstIterator) {
//     Graph<int> graph;
//     graph.add_vertex(1);
//     graph.add_vertex(2);
//     graph.add_vertex(3);
//
//     auto it = graph.vertices_begin();
//     std::list<VertexNode>::const_iterator constIt = graph.vertex_list().begin();
//
//     ASSERT_TRUE(it == constIt);
//     ++it;
//     ++constIt;
//     ASSERT_TRUE(it == constIt);
//     ASSERT_FALSE(it != constIt);
// }

TEST(VertexIteratorTest, PostfixIncrement) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);

    auto it = graph.vertices_begin();
    auto temp = it++;
    ASSERT_EQ(*temp, 1);
    ASSERT_EQ(*it, 2);
}

TEST(VertexIteratorTest, PostfixDecrement) {
    Graph<int> graph;
    graph.add_vertex(1);
    graph.add_vertex(2);

    auto it = graph.vertices_end();
    --it; // Чтобы попасть на последний элемент
    auto temp = it--;
    ASSERT_EQ(*temp, 2);

    ASSERT_EQ(*it, 1);
}
