#include "InStoreCustomer.h"
#include <gtest/gtest.h>
#include <string>


TEST(Person, GetName)
{
    InStoreCustomer person("Ivan", 30, 1001, 500, "12345-67890");
    EXPECT_EQ(person.getName(), "Ivan");
}
TEST(Person, GetAge)
{
    InStoreCustomer person("Ivan", 30, 1001, 500, "12345-67890");
    EXPECT_EQ(person.getAge(), 30);
}