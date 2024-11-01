#include "DeliveryPerson.h"
#include "Order.h"
#include "Pizza.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Проверка функции deliverOrder
TEST(DeliveryPersonTest, DeliverOrder) {
    DeliveryPerson deliveryPerson("Андрей", 28, 2001, 3000.0, "Bike");
    Order order(1, nullptr);
    Pizza pizza("Large", "Thin Crust", 15.0);
    order.addPizza(pizza);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    deliveryPerson.deliverOrder(order);

    std::cout.rdbuf(old);

    std::string expected_output = "Доставляем заказ с ID: 1 используя Bike.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(order.getStatus(), "Delivered");
    EXPECT_EQ(deliveryPerson.getVehicleType(),"Bike");
}

// Проверка функции trackOrder
TEST(DeliveryPersonTest, TrackOrder) {
    DeliveryPerson deliveryPerson("Андрей", 28, 2001, 3000.0, "Bike");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    deliveryPerson.trackOrder("1");

    std::cout.rdbuf(old);

    std::string expected_output = "Отслеживаем заказ с ID: 1.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции calculateBonus
TEST(DeliveryPersonTest, CalculateBonus) {
    DeliveryPerson deliveryPerson("Андрей", 28, 2001, 3000.0, "Bike");
    double bonus = deliveryPerson.calculateBonus();
    EXPECT_DOUBLE_EQ(bonus, 3000.0 * 0.05);
}

// Проверка функции work
TEST(DeliveryPersonTest, Work) {
    DeliveryPerson deliveryPerson("Андрей", 28, 2001, 3000.0, "Bike");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    deliveryPerson.work();

    std::cout.rdbuf(old);

    std::string expected_output = "Курьер работает над доставкой заказов\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

