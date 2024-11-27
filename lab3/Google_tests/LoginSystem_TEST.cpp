#include <gtest/gtest.h>
#include "LoginSystem.h"
#include <stdexcept> // Включение заголовка для std::invalid_argument

TEST(LoginSystemTest, RegisterAndLoginTest) {
    LoginSystem loginSystem;

    // Регистрация нового пользователя
    loginSystem.registerUser("user1", "password1");
    EXPECT_TRUE(loginSystem.login("user1", "password1"));
    EXPECT_FALSE(loginSystem.login("user1", "wrongpassword"));
    EXPECT_FALSE(loginSystem.login("nonexistentuser", "password1"));
}

TEST(LoginSystemTest, RegisterDuplicateUserTest) {
    LoginSystem loginSystem;

    // Регистрация нового пользователя
    loginSystem.registerUser("user1", "password1");

    // Попытка повторной регистрации того же пользователя должна вызвать исключение
    EXPECT_THROW(loginSystem.registerUser("user1", "password1"), std::invalid_argument);
}


