#include "CashPayment.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Проверка функции processPayment с достаточным количеством наличных
TEST(CashPaymentTest, ProcessPaymentSufficientCash) {
    CashPayment payment(1, 500.0, 600.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж успешно обработан. Сдача: 100 рублей.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции processPayment с недостаточным количеством наличных
TEST(CashPaymentTest, ProcessPaymentInsufficientCash) {
    CashPayment payment(1, 500.0, 400.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Недостаточно наличных для оплаты.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции refundPayment
TEST(CashPaymentTest, RefundPayment) {
    CashPayment payment(1, 500.0, 600.0);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.refundPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Возврат платежа в размере 500 рублей успешно произведен.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции wasSuccessful с достаточным количеством наличных
TEST(CashPaymentTest, WasSuccessfulSufficientCash) {
    CashPayment payment(1, 500.0, 600.0);
    EXPECT_TRUE(payment.wasSuccesfull());
}

// Проверка функции wasSuccessful с недостаточным количеством наличных
TEST(CashPaymentTest, WasSuccessfulInsufficientCash) {
    CashPayment payment(1, 500.0, 400.0);
    EXPECT_FALSE(payment.wasSuccesfull());
}


