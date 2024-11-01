#include "LoyaltyProgram.h"
#include <gtest/gtest.h>
#include <sstream>

// Тестирование функции applyDiscount
TEST(LoyaltyProgramTest, ApplyDiscount) {
    LoyaltyProgram program(1, 0.1); // 10% скидка
    double price = 100.0;
    program.applyDiscount(price);
    EXPECT_DOUBLE_EQ(price, 90.0);
}

// Тестирование функции getDiscountRate
TEST(LoyaltyProgramTest, GetDiscountRate) {
    LoyaltyProgram program(1, 0.1); // 10% скидка
    EXPECT_DOUBLE_EQ(program.getdiscountRate(), 0.1);
}

