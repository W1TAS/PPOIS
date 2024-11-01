#include "InStoreCustomer.h"
#include <gtest/gtest.h>
#include <string>

// Тестирование конструктора и функции getMembershipCard
TEST(InStoreCustomerTest, ConstructorAndGetMembershipCard) {
    InStoreCustomer customer("Ivan Ivanov", 30, 1001, 500, "12345-67890");

    EXPECT_EQ(customer.getName(), "Ivan Ivanov");
    EXPECT_EQ(customer.getAge(), 30);
    EXPECT_EQ(customer.getCustomerID(), 1001);
    EXPECT_EQ(customer.getLoyaltyPoints(), 500);
    EXPECT_EQ(customer.getMembershipCard(), "12345-67890");
}

// Тестирование функции placeOrder
TEST(InStoreCustomerTest, PlaceOrder) {
    InStoreCustomer customer("Ivan Ivanov", 30, 1001, 500, "12345-67890");
    Order order(1, &customer);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.placeOrder(order);

    std::cout.rdbuf(old);

    std::string expected_output = "Оформляется заказ с ID: 1.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getStatus(), "Placed");
}

// Тестирование функции useLoyaltyPoints
TEST(InStoreCustomerTest, UseLoyaltyPoints) {
    InStoreCustomer customer("Ivan Ivanov", 30, 1001, 500, "12345-67890");
    Order order(1, &customer);
    Pizza pizza("Large", "Thin Crust", 100.0);
    order.addPizza(pizza);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.useLoyaltyPoints(200, order);

    std::cout.rdbuf(old);

    std::string expected_output = "200 бонусных баллов. Новая цена: 90. Оставшиеся бонусные баллы: 300.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getTotalPrice(), 90);
    EXPECT_EQ(customer.getLoyaltyPoints(), 300);
}

// Тестирование функции useLoyaltyProgram
TEST(InStoreCustomerTest, UseLoyaltyProgram) {
    InStoreCustomer customer("Ivan Ivanov", 30, 1001, 500, "12345-67890");
    Order order(1, &customer);
    Pizza pizza("Large", "Thin Crust", 100.0);
    order.addPizza(pizza);

    LoyaltyProgram loyaltyProgram(1, 0.1); // Программа с 10% скидкой

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.useLoyaltyProgram(loyaltyProgram, order);

    std::cout.rdbuf(old);

    std::string expected_output = "Скидка применена. Новая цена: 90.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getTotalPrice(), 90);
}

