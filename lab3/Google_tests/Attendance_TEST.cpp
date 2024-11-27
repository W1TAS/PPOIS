#include <gtest/gtest.h>
#include "Attendance.h"
#include "Employee.h"

TEST(AttendanceTest, ConstructorTest) {
    Employee emp(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
    Attendance attendance(&emp, "2024-11-25", true);

    EXPECT_EQ(attendance.getEmployee()->getFirstName(), "John");
    EXPECT_EQ(attendance.getDate(), "2024-11-25");
    EXPECT_TRUE(attendance.isPresent());
}

TEST(AttendanceTest, UpdateAttendanceTest) {
    Employee emp(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
    Attendance attendance(&emp, "2024-11-25", true);
    attendance.updateAttendance(false);

    EXPECT_FALSE(attendance.isPresent());
}

