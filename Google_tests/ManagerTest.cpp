#include "Manager.h"
#include "DeliveryPerson.h"
#include "Chef.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Тестирование функции getManagedEmployees
TEST(ManagerTest, GetManagedEmployees) {
    Manager manager("Alice", 40, 1001, 5000.0);
    EXPECT_TRUE(manager.getManagedEmployees().empty());
}

// Тестирование функции addEmployee и getManagedEmployees
TEST(ManagerTest, AddEmployee) {
    Manager manager("Alice", 40, 1001, 5000.0);
    DeliveryPerson employee("John", 35, 1002, 3000.0, "Bike");
    manager.addEmployee(&employee);
    EXPECT_EQ(manager.getManagedEmployees().size(), 1);
    EXPECT_EQ(manager.getManagedEmployees()[0]->getEmployeeID(), 1002);
}

// Тестирование функции removeEmployee
TEST(ManagerTest, RemoveEmployee) {
    Manager manager("Alice", 40, 1001, 5000.0);
    Chef employee("John", 35, 1002, 3000.0, "French");
    manager.addEmployee(&employee);
    manager.removeEmployee(&employee);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    manager.removeEmployee(&employee);

    std::cout.rdbuf(old);

    std::string expected_output = "Работник не найден.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_TRUE(manager.getManagedEmployees().empty());
}

// Тестирование функции calculateBonus
TEST(ManagerTest, CalculateBonus) {
    Manager manager("Alice", 40, 1001, 5000.0);
    EXPECT_DOUBLE_EQ(manager.calculateBonus(), 500.0);
}

// Тестирование функции work
TEST(ManagerTest, Work) {
    Manager manager("Alice", 40, 1001, 5000.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    manager.work();

    std::cout.rdbuf(old);

    std::string expected_output = "Менеджер работает.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

