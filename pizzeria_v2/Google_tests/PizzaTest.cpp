#include <gtest/gtest.h>
#include "Pizza.h"

TEST(PizzaTest, DefaultConstructor) {
    Pizza pizza("Medium", "Thin Crust", 10);
    EXPECT_EQ(pizza.getSize(), "Medium");
    EXPECT_EQ(pizza.getCrustType(), "Thin Crust");
    EXPECT_EQ(pizza.getPrice(), 10);
    EXPECT_TRUE(pizza.getToppings().empty());
}

TEST(PizzaTest, AddTopping) {
    Pizza pizza("Medium", "Thin Crust", 15);
    pizza.addTopping("Pepperoni");
    EXPECT_EQ(pizza.getToppings().size(), 1);
    EXPECT_EQ(pizza.getToppings()[0], "Pepperoni");
}

TEST(PizzaTest, EqualityOperator) {
    Pizza pizza1("Medium", "Thin Crust", 15);
    Pizza pizza2("Medium", "Thin Crust", 15);
    EXPECT_EQ(pizza1, pizza2);
}
