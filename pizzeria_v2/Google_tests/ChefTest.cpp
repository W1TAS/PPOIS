#include "Chef.h"
#include "Order.h"
#include "Pizza.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Тестирование функции prepareOrder для вывода информации о приготовлении пиццы
TEST(ChefTest, PrepareOrderOutputTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");
    Order order(1, nullptr);
    Pizza pizza1("Large", "Thin Crust", 16.0);
    pizza1.addTopping("Cheese");
    pizza1.addTopping("Tomato");
    Pizza pizza2("Medium", "Thick Crust", 12.0);
    pizza2.addTopping("Pepperoni");
    pizza2.addTopping("Mushroom");
    order.addPizza(pizza1);
    order.addPizza(pizza2);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    chef.prepareOrder(order);

    std::cout.rdbuf(old);

    std::string expected_output = "Шеф-повар Виктор Баринов готовит пиццу с размером Large и типом корочки Thin Crust с топпингами: Cheese Tomato.\n"
                                  "Шеф-повар Виктор Баринов готовит пиццу с размером Medium и типом корочки Thick Crust с топпингами: Pepperoni Mushroom.\n"
                                  "Все пиццы в заказе приготовлены. Статус заказа: Completed.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Тестирование функции prepareOrder для обновления статуса заказа
TEST(ChefTest, PrepareOrderStatusTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");
    Order order(1, nullptr);
    Pizza pizza("Large", "Thin Crust", 16.0);
    order.addPizza(pizza);

    chef.prepareOrder(order);

    EXPECT_EQ(order.getStatus(), "Completed");
}

// Тестирование функции prepareOrder для вывода информации о конкретной пицце
TEST(ChefTest, PrepareOrderSpecificPizzaTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");
    Order order(1, nullptr);
    Pizza pizza("Large", "Thin Crust", 16.0);
    pizza.addTopping("Cheese");
    order.addPizza(pizza);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    chef.prepareOrder(order);

    std::cout.rdbuf(old);

    std::string expected_output = "Шеф-повар Виктор Баринов готовит пиццу с размером Large и типом корочки Thin Crust с топпингами: Cheese.\n"
                                  "Все пиццы в заказе приготовлены. Статус заказа: Completed.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}




// Проверка функции createRecipe
TEST(ChefTest, CreateRecipeTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    chef.createRecipe("Паста Карбонара");

    std::cout.rdbuf(old);

    std::string expected_output = "Шеф-повар Виктор Баринов создал новый рецепт: Паста Карбонара.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции calculateBonus
TEST(ChefTest, CalculateBonusTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");
    double bonus = chef.calculateBonus();
    EXPECT_DOUBLE_EQ(bonus, 3000.0 * 0.15);
}

// Проверка функции work
TEST(ChefTest, WorkTest) {
    Chef chef("Виктор Баринов", 35, 3002, 3000.0, "Italian Cuisine");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    chef.work();

    std::cout.rdbuf(old);

    std::string expected_output = "Шеф-повар Виктор Баринов создал новый рецепт: Специальное блюдо от шефа.\nШеф-повар Виктор Баринов готовит блюда и управляет кухней.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_EQ(chef.getSpecialty(), "Italian Cuisine");
}


