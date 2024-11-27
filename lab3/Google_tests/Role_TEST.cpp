#include <gtest/gtest.h>
#include "Role.h"

TEST(RoleTest, ConstructorTest) {
    Role role("Admin", "Full Access");

    EXPECT_EQ(role.getName(), "Admin");
    EXPECT_EQ(role.getPermissions(), "Full Access");
}

TEST(RoleTest, UpdateRoleTest) {
    Role role("Admin", "Full Access");
    role.updateRole("User", "Limited Access");

    EXPECT_EQ(role.getName(), "User");
    EXPECT_EQ(role.getPermissions(), "Limited Access");
}

