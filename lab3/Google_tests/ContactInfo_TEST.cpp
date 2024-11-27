#include <gtest/gtest.h>
#include "ContactInfo.h"

TEST(ContactInfoTest, ConstructorTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");

    EXPECT_EQ(contact.getPhone(), "123-456-7890");
    EXPECT_EQ(contact.getEmail(), "john.doe@example.com");
}

TEST(ContactInfoTest, UpdateContactInfoTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    contact.updateContactInfo("098-765-4321", "jane.doe@example.com");

    EXPECT_EQ(contact.getPhone(), "098-765-4321");
    EXPECT_EQ(contact.getEmail(), "jane.doe@example.com");
}


