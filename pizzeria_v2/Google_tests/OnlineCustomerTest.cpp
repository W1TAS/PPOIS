#include "OnlineCustomer.h"
#include <gtest/gtest.h>
#include <sstream>

// Тестирование конструктора и метода getEmail
TEST(OnlineCustomerTest, GetEmail) {
    OnlineCustomer customer("Alice", 25, 1001, 200, "alice@example.com", "password123");
    EXPECT_EQ(customer.getEmail(), "alice@example.com");
}

// Тестирование метода checkPassword с правильным паролем
TEST(OnlineCustomerTest, CheckPasswordCorrect) {
    OnlineCustomer customer("Alice", 25, 1001, 200, "alice@example.com", "password123");
    EXPECT_TRUE(customer.checkPassword("password123"));
}

// Тестирование метода checkPassword с неправильным паролем
TEST(OnlineCustomerTest, CheckPasswordIncorrect) {
    OnlineCustomer customer("Alice", 25, 1001, 200, "alice@example.com", "password123");
    EXPECT_FALSE(customer.checkPassword("wrongpassword"));
}

// Тестирование метода resetPassword
TEST(OnlineCustomerTest, ResetPassword) {
    OnlineCustomer customer("Alice", 25, 1001, 200, "alice@example.com", "password123");
    customer.resetPassword("newpassword");
    EXPECT_TRUE(customer.checkPassword("newpassword"));
    EXPECT_FALSE(customer.checkPassword("password123"));
}

