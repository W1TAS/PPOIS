#include "BankTransferPayment.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Проверка функции processPayment с успешным банковским номером
TEST(BankTransferPaymentTest, ProcessPaymentSuccessfulTest) {
    BankTransferPayment payment(1, 1000.0, "1234567890");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Платеж на сумму 1000 рублей успешно переведен на счет 1234567890.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции processPayment с неуспешным банковским номером
TEST(BankTransferPaymentTest, ProcessPaymentUnsuccessfulTest) {
    BankTransferPayment payment(1, 1000.0, "123");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    payment.processPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Ошибка: недействительный банковский номер счета.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции refundPayment после успешного перевода
TEST(BankTransferPaymentTest, RefundPaymentSuccessfulTest) {
    BankTransferPayment payment(1, 1000.0, "1234567890");

    // Выполняем перевод
    payment.processPayment();

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Выполняем возврат
    payment.refundPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Возврат платежа на сумму 1000 рублей успешно произведен на счет 1234567890.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции refundPayment после неуспешного перевода
TEST(BankTransferPaymentTest, RefundPaymentUnsuccessfulTest) {
    BankTransferPayment payment(1, 1000.0, "123");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    // Выполняем возврат
    payment.refundPayment();

    std::cout.rdbuf(old);

    std::string expected_output = "Ошибка: недействительный банковский номер счета.\n";
    EXPECT_EQ(buffer.str(), expected_output);
}

// Проверка функции wasSuccessful с валидным банковским номером
TEST(BankTransferPaymentTest, WasSuccessfulValidAccountNumber) {
    BankTransferPayment payment(1, 1000.0, "1234567890");
    EXPECT_TRUE(payment.wasSuccesfull());
}

// Проверка функции wasSuccessful с невалидным банковским номером
TEST(BankTransferPaymentTest, WasSuccessfulInvalidAccountNumber) {
    BankTransferPayment payment(1, 1000.0, "123");
    EXPECT_FALSE(payment.wasSuccesfull());
}

