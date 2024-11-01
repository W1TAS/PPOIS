#include "CashPayment.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Проверка функции processPayment с достаточным количеством наличных
TEST(PaymentTest, SetAmont) {
    CashPayment payment(1, 500.0, 600.0);

    payment.setAmount(400);

    EXPECT_EQ(payment.getAmmount(), 400);
}



