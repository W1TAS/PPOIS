#include "DeliveryPerson.h"
#include <gtest/gtest.h>
#include <string>


TEST(EmployeeTest, EmployeeGetters)
{
    DeliveryPerson deliveryPerson("Андрей", 28, 2001, 3000.0, "Bike");
    EXPECT_EQ(deliveryPerson.getEmployeeID(), 2001);
    EXPECT_EQ(deliveryPerson.getSalary(), 3000);
}

