#include "Order.h"
#include "Pizza.h"
#include "Customer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Тестирование конструктора и метода getOrderID
TEST(OrderTest, GetOrderID) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    EXPECT_EQ(order.getOrderID(), 1);
}

// Тестирование метода getCustomer
TEST(OrderTest, GetCustomer) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    EXPECT_EQ(order.getCustomer(), customer);
}

// Тестирование метода addPizza и calculateTotalPrice
TEST(OrderTest, AddPizza) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 16.0);
    Pizza pizza2("Medium", "Thick Crust", 12.0);
    order.addPizza(pizza1);
    order.addPizza(pizza2);
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 28.0);
}

// Тестирование метода getStatus и setStatus
TEST(OrderTest, SetAndGetStatus) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    order.setStatus("Delivered");
    EXPECT_EQ(order.getStatus(), "Delivered");
}

// Тестирование метода removePizza
TEST(OrderTest, RemovePizza) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 16.0);
    Pizza pizza2("Medium", "Thick Crust", 12.0);
    order.addPizza(pizza1);
    order.addPizza(pizza2);
    order.removePizza(pizza1);
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 12.0);
}

// Тестирование метода updatePizza
TEST(OrderTest, UpdatePizza) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 16.0);
    Pizza pizza2("Medium", "Thick Crust", 12.0);
    order.addPizza(pizza1);
    order.updatePizza(pizza1, pizza2);
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 12.0);
}

// Тестирование метода applyDiscount
TEST(OrderTest, ApplyDiscount) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    Pizza pizza("Large", "Thin Crust", 100.0);
    order.addPizza(pizza);
    order.applyDiscount(0.1); // 10% скидка
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 90.0);
}

// Тестирование метода applyLoyaltyPoints
TEST(OrderTest, ApplyLoyaltyPoints) {
    Customer* customer = nullptr; // Поскольку у нас нет реализации Customer, используем nullptr
    Order order(1, customer);
    Pizza pizza("Large", "Thin Crust", 100.0);
    order.addPizza(pizza);
    int pointsApplied = order.applyLoyaltyPoints(2000); // 2000 бонусных очков
    EXPECT_EQ(pointsApplied, 2000);
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 0.0);
}

// Тестирование функции applyLoyaltyPoints с недостаточными баллами
TEST(OrderTest, ApplyLoyaltyPointsInsufficient) {
    Customer* customer = nullptr;  // For simplicity, we use a nullptr here
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 200.0);
    order.addPizza(pizza1);

    int points = order.applyLoyaltyPoints(100); // 100 points = 5 discount
    EXPECT_EQ(points, 100);
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 195.0); // 200 - 5 = 195
}

// Тестирование функции applyLoyaltyPoints с достаточными баллами
TEST(OrderTest, ApplyLoyaltyPointsSufficient) {
    Customer* customer = nullptr;  // For simplicity, we use a nullptr here
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 100.0);
    order.addPizza(pizza1);

    int points = order.applyLoyaltyPoints(3000); // 3000 points = 150 discount
    EXPECT_EQ(points, 2000); // only 2000 points can be applied to cover 100 * 2 = 200 total price
    EXPECT_DOUBLE_EQ(order.getTotalPrice(), 0.0);
}

// Тестирование функции getPizzas
TEST(OrderTest, GetPizzas) {
    Customer* customer = nullptr;  // For simplicity, we use a nullptr here
    Order order(1, customer);
    Pizza pizza1("Large", "Thin Crust", 200.0);
    Pizza pizza2("Medium", "Thick Crust", 150.0);
    order.addPizza(pizza1);
    order.addPizza(pizza2);

    std::vector<Pizza> pizzas = order.getPizzas();
    ASSERT_EQ(pizzas.size(), 2);
    EXPECT_EQ(pizzas[0].getSize(), "Large");
    EXPECT_EQ(pizzas[0].getCrustType(), "Thin Crust");
    EXPECT_EQ(pizzas[1].getSize(), "Medium");
    EXPECT_EQ(pizzas[1].getCrustType(), "Thick Crust");
}

