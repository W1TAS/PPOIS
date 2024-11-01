#include "Customer.h"
#include "CreditCardPayment.h"
#include "CashPayment.h"
#include "LoyaltyProgram.h"
#include "Order.h"
#include "Pizza.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Тестирование конструктора
TEST(CustomerTest, Constructor) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
    EXPECT_EQ(customer.getCustomerID(), 1001);
    EXPECT_EQ(customer.getLoyaltyPoints(), 500);
}

// Тестирование функции placeOrder
TEST(CustomerTest, PlaceOrder) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
    Order order(1, &customer);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.placeOrder(order);

    std::cout.rdbuf(old);

    std::string expected_output = "Оформляется заказ с ID: 1.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getStatus(), "Placed");
}

// Тестирование функции makePayment с успешной оплатой
TEST(CustomerTest, MakePaymentSuccessful) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
    CashPayment payment(1, 200.0, 300.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.makePayment(payment);

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж успешно обработан. Сдача: 100 рублей.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(customer.getLoyaltyPoints(), 700); // Проверка обновления бонусных очков
}

// Тестирование функции makePayment с неуспешной оплатой
TEST(CustomerTest, MakePaymentUnsuccessful) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
    CashPayment payment(1, 300.0, 100.0); // Недостаточно наличных для оплаты

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.makePayment(payment);

    std::cout.rdbuf(old);

    std::string expected_output = "Платёж не одобрен.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(customer.getLoyaltyPoints(), 500); // Проверка, что бонусные очки не изменились
}


TEST(CustomerTest, UseLoyaltyPointsSuccessful) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
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

TEST(CustomerTest, UseLoyaltyPointsUnsuccessful) {
    Customer customer("Ivan Ivanov", 30, 1001, 100);
    Order order(1, &customer);
    Pizza pizza("Large", "Thin Crust", 100.0);
    order.addPizza(pizza);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    customer.useLoyaltyPoints(200, order);

    std::cout.rdbuf(old);

    std::string expected_output = "Неправильное значение бонусных очков\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getTotalPrice(), 100);
    EXPECT_EQ(customer.getLoyaltyPoints(), 100);
}


TEST(CustomerTest, UseLoyaltyProgram) {
    Customer customer("Ivan Ivanov", 30, 1001, 500);
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


