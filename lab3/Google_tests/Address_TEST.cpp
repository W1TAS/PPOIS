#include <gtest/gtest.h>
#include "../../lab3/salary_lib/Address.h"

TEST(AddressTest, ConstructorTest) {
    Address address("Minsk", "Lenina", 10);

    EXPECT_EQ(address.getCity(), "Minsk");
    EXPECT_EQ(address.getStreet(), "Lenina");
    EXPECT_EQ(address.getHouseNumber(), 10);
}

TEST(AddressTest, UpdateAddressTest) {
    Address address("Minsk", "Lenina", 10);
    address.updateAddress("Minsk", "Pushkina", 15);

    EXPECT_EQ(address.getCity(), "Minsk");
    EXPECT_EQ(address.getStreet(), "Pushkina");
    EXPECT_EQ(address.getHouseNumber(), 15);
}


