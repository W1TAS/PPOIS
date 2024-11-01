#include "CreditCardPayment.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Проверка функции processPayment с валидной картой и достаточным балансом
TEST(CreditCardPaymentTest, ProcessPaymentSuccessful) {
    CreditCardPayment payment(1, 1000.0, "1234567812345678", "12/25", 2000.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж на сумму 1000 рублей успешно обработан.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_DOUBLE_EQ(payment.getBalance(), 1000.0); // Проверка обновленного баланса
}

// Проверка функции processPayment с недостаточным балансом
TEST(CreditCardPaymentTest, ProcessPaymentInsufficientBalance) {
    CreditCardPayment payment(1, 1000.0, "1234567812345678", "12/25", 500.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж отклонен.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_DOUBLE_EQ(payment.getBalance(), 500.0); // Проверка, что баланс не изменился
}

// Проверка функции processPayment с невалидной картой
TEST(CreditCardPaymentTest, ProcessPaymentInvalidCard) {
    CreditCardPayment payment(1, 1000.0, "1234", "12/25", 2000.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж отклонен.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_DOUBLE_EQ(payment.getBalance(), 2000.0); // Проверка, что баланс не изменился
}

// Проверка функции refundPayment с валидной картой
TEST(CreditCardPaymentTest, RefundPaymentValidCard) {
    CreditCardPayment payment(1, 1000.0, "1234567812345678", "12/25", 1000.0);

    // Выполняем первоначальный платеж
    payment.processPayment();

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Выполняем возврат
    payment.refundPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Возврат платежа на сумму 1000 рублей успешно произведен.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_DOUBLE_EQ(payment.getBalance(), 1000.0); // Проверка, что баланс восстановлен
}

// Проверка функции refundPayment с невалидной картой
TEST(CreditCardPaymentTest, RefundPaymentInvalidCard) {
    CreditCardPayment payment(1, 1000.0, "1234", "12/25", 1000.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Выполняем возврат
    payment.refundPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Возврат отклонен из-за ошибки валидации карты.\n";
    EXPECT_EQ(buffer.str(), expected_output);
    EXPECT_DOUBLE_EQ(payment.getBalance(), 1000.0); // Проверка, что баланс не изменился
}

// Проверка функции wasSuccessful с валидной картой и достаточным балансом
TEST(CreditCardPaymentTest, WasSuccessfulValid) {
    CreditCardPayment payment(1, 1000.0, "1234567812345678", "12/25", 2000.0);
    EXPECT_TRUE(payment.wasSuccesfull());
}

// Проверка функции wasSuccessful с невалидной картой
TEST(CreditCardPaymentTest, WasSuccessfulInvalidCard) {
    CreditCardPayment payment(1, 1000.0, "1234", "12/25", 2000.0);
    EXPECT_FALSE(payment.wasSuccesfull());
}

// Проверка функции wasSuccessful с недостаточным балансом
TEST(CreditCardPaymentTest, WasSuccessfulInsufficientBalance) {
    CreditCardPayment payment(1, 1000.0, "1234567812345678", "12/25", 500.0);
    EXPECT_FALSE(payment.wasSuccesfull());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
