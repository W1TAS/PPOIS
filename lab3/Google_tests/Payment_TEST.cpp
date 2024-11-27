#include <gtest/gtest.h>
#include "Payment.h"
#include "Employee.h"
#include "ContactInfo.h"
#include "Address.h"

TEST(PaymentTest, ConstructorTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    Payment payment(&emp, 1000, "2024-11-25");

    EXPECT_EQ(payment.getEmployee()->getFirstName(), "John");
    EXPECT_DOUBLE_EQ(payment.getAmount(), 1000);
    EXPECT_EQ(payment.getDate(), "2024-11-25");
}

TEST(PaymentTest, NegativeAmountTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    // Попытка создания платежа с отрицательной суммой должна вызвать исключение
    EXPECT_THROW(Payment(&emp, -1000, "2024-11-25"), std::invalid_argument);
}

TEST(PaymentTest, ProcessPaymentTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    Payment payment(&emp, 1000, "2024-11-25");

    // Перехват стандартного вывода
    testing::internal::CaptureStdout();
    payment.processPayment();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "Processed payment of $1000 to John Doe on 2024-11-25\n");
}

